// Based on https://www.geeksforgeeks.org/all-topological-sorts-of-a-directed-acyclic-graph/

// [[Rcpp::depends(RcppArmadillo)]]

#include <RcppArmadillo.h>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <filesystem>
using namespace std;

class Graph {
  int n_items;
  std::vector<std::list<int>> adj;
  vector<int> indegree;
  void alltopologicalSortUtil(
      vector<int>& res, vector<bool>& visited, long long int& sort_count,
      std::vector<arma::ivec>& sorts, double save_frac);

public:
  Graph(int n_items);
  void addEdge(int v, int w);
  arma::imat alltopologicalSort(long long int& sort_count, double save_frac);
};

Graph::Graph(int n_items) : n_items { n_items }, adj(n_items),
indegree(n_items, 0) {}

void Graph::addEdge(int v, int w) {
  adj[v].push_back(w);
  indegree[w]++;
}

void Graph::alltopologicalSortUtil(
    vector<int>& res, vector<bool>& visited, long long int& sort_count,
    std::vector<arma::ivec>& sorts, double save_frac) {
  bool flag = false;

  for (size_t i{}; i < n_items; i++) {
    if (indegree[i] == 0 && !visited[i]) {
      list<int>::iterator j;
      for (j = adj[i].begin(); j != adj[i].end(); j++)
        indegree[*j]--;

      res.push_back(i);
      visited[i] = true;
      alltopologicalSortUtil(res, visited, sort_count, sorts, save_frac);

      visited[i] = false;
      res.erase(res.end() - 1);
      for (j = adj[i].begin(); j != adj[i].end(); j++)
        indegree[*j]++;

      flag = true;
    }
  }

  if (!flag){
    if(R::runif(0, 1) < save_frac) {
      arma::ivec sort_vector(res.size());
      for(size_t i = 0; i < res.size(); ++i) {
        sort_vector(i) = res[i] + 1; // converting to 1-based indexing
      }
      sorts.push_back(sort_vector);
    }
    sort_count++;
  }
}

arma::imat Graph::alltopologicalSort(long long int& sort_count, double save_frac) {
  vector<bool> visited(n_items, false);
  vector<int> res;
  std::vector<arma::ivec> sorts;
  alltopologicalSortUtil(res, visited, sort_count, sorts, save_frac);

  if(sorts.empty()) {
    return arma::imat(0, 0);
  }

  arma::imat sort_matrix(sorts[0].n_elem, sorts.size());
  for (size_t i = 0; i < sorts.size(); ++i) {
    sort_matrix.col(i) = sorts[i];
  }

  return sort_matrix;
}

//' Precompute All Topological Sorts
//'
//' This function precomputes all topological sorts for a given preference matrix.
//' Topological sorts are consistent orderings of items that respect the given
//' pairwise preference constraints.
//'
//' @param prefs A matrix representing the preference relations. This matrix
//'   must have two columns, the first of which represents the preferred item
//'   and the second of which represents the disfavored item.
//' @param n_items An integer specifying the number of items to sort.
//' @param save_frac Number between 0 and 1 specifying which fraction of sorts to save.
//'
//' @details
//' The function generates all possible topological sorts for the provided preference matrix
//' and saves approximately \code{save_frac} of the sorts in a matrix which is returned.
//'
//' @return A list with two elements:
//' \describe{
//'   \item{sort_count}{An integer giving the total number of topological sorts.}
//'   \item{sort_matrix}{A matrix where each column represents one topological sort.
//'     The number of columns is approximately \code{save_frac} times \code{sort_count}.
//'     If \code{save_frac = 0}, this is an empty matrix with dimensions \code{c(0, 0)}.}
//' }
//'
//' @export
//' @examples
//' # Extract preferences from user 1 in the included example data.
//' prefs <- pairwise_preferences[
//'  pairwise_preferences$user == 1, c("top_item", "bottom_item"), drop = FALSE]
//'
//' # Generate all topological sorts, but don't save them:
//' sorts <- precompute_topological_sorts(
//'   prefs = as.matrix(prefs),
//'   n_items = 5,
//'   save_frac = 0
//' )
//' # Number of sorts
//' sorts$sort_count
//' # Empty matrix
//' sorts$sort_matrix
//'
//' # Generate all topological sorts and save them:
//' sorts <- precompute_topological_sorts(
//'   prefs = as.matrix(prefs),
//'   n_items = 5,
//'   save_frac = 1
//' )
//' # Number of sorts
//' sorts$sort_count
//' # Matrix with all of them
//' sorts$sort_matrix
//'
// [[Rcpp::export]]
Rcpp::List precompute_topological_sorts(
   arma::umat prefs, int n_items, double save_frac) {
 Graph g(n_items);
 for(size_t i{}; i < prefs.n_rows; i++) {
   g.addEdge(prefs.at(i, 0) - 1, prefs.at(i, 1) - 1);
 }

 long long int sort_count = 0;
 arma::imat sort_matrix = g.alltopologicalSort(sort_count, save_frac);

 return Rcpp::List::create(
   Rcpp::Named("sort_count") = sort_count,
   Rcpp::Named("sort_matrix") = sort_matrix
 );
}
