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

  arma::imat sort_matrix(sorts.size(), sorts[0].n_elem);
  for (size_t i = 0; i < sorts.size(); ++i) {
    sort_matrix.row(i) = sorts[i].t();
  }

  return sort_matrix;
}

//' Precompute All Topological Sorts
//'
//' This function precomputes all topological sorts for a given preference matrix and
//' saves them to a specified output directory. It ensures the output directory exists
//' and creates it if it does not.
//'
//' @param prefs A matrix representing the preference relations. This matrix
//'   must have two columns, the first of which represents the preferred item
//'   and the second of which represents the disfavored item.
//' @param n_items An integer specifying the number of items to sort.
//' @param output_directory A string specifying the directory where the output files will be saved.
//' @param save_frac Number between 0 and 1 specifying which fraction of sorts to save.
//'
//' @details
//' The function generates all possible topological sorts for the provided preference matrix
//' and saves approximately `save_frac` of the sorts as binary file in the specified output directory.
//' The output files are named sequentially as `sort0.bin`, `sort1.bin`, and so on.
//'
//' @return This function returns the number of topological sorts.
//'
//' @export
//' @examples
//' # Extract preferences from user 1 in the included example data.
//' prefs <- pairwise_preferences[
//'  pairwise_preferences$user == 1, c("top_item", "bottom_item"), drop = FALSE]
//'
//' # Count the number of sorts without saving them.
//' precompute_topological_sorts(
//'   prefs = as.matrix(prefs),
//'   n_items = 5,
//'   output_directory = tempdir(),
//'   save_frac = 0
//' )
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
