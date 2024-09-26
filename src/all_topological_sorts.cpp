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
      vector<int>& res, vector<bool>& visited, const std::string& output_directory,
      long long int& sort_count, double save_frac);

public:
  Graph(int n_items);
  void addEdge(int v, int w);
  void alltopologicalSort(const std::string& output_directory, long long int& sort_count, double save_frac);
};

Graph::Graph(int n_items) : n_items { n_items }, adj(n_items),
indegree(n_items, 0) {}

void Graph::addEdge(int v, int w) {
  adj[v].push_back(w);
  indegree[w]++;
}

void Graph::alltopologicalSortUtil(vector<int>& res, vector<bool>& visited,
                                   const std::string& output_directory, long long int& sort_count,
                                   double save_frac) {
  bool flag = false;

  for (size_t i{}; i < n_items; i++) {
    if (indegree[i] == 0 && !visited[i]) {
      list<int>::iterator j;
      for (j = adj[i].begin(); j != adj[i].end(); j++)
        indegree[*j]--;

      res.push_back(i);
      visited[i] = true;
      alltopologicalSortUtil(res, visited, output_directory, sort_count, save_frac);

      visited[i] = false;
      res.erase(res.end() - 1);
      for (j = adj[i].begin(); j != adj[i].end(); j++)
        indegree[*j]++;

      flag = true;
    }
  }

  if (!flag){
    if(R::runif(0, 1) < save_frac) {
      std::ostringstream filename_stream;
      filename_stream << output_directory << "/sort" << sort_count << ".bin";
      std::string filename = filename_stream.str();

      // Save the current found topological sort to file
      arma::ivec sort_vector(res.size());
      for(size_t i = 0; i < res.size(); ++i) {
        sort_vector(i) = res[i] + 1; // converting to 1-based indexing
      }
      sort_vector.save(filename);
    }
    sort_count++;
  }
}

void Graph::alltopologicalSort(const std::string& output_directory,
                               long long int& sort_count,
                               double save_frac) {
  vector<bool> visited(n_items, false);
  vector<int> res;
  alltopologicalSortUtil(res, visited, output_directory, sort_count, save_frac);
}

//' Precompute All Topological Sorts
//'
//' This function precomputes all topological sorts for a given preference matrix and
//' saves them to a specified output directory. It ensures the output directory exists
//' and creates it if it does not.
//'
//' @param prefs An integer matrix (`arma::umat`) representing the preference relations.
//' @param n_items An integer specifying the number of items to sort.
//' @param output_directory A string specifying the directory where the output files will be saved.
//' @param save_frac Number between 0 and 1 specifying which fraction of sorts to save.
//'
//' @details
//' The function generates all possible topological sorts for the provided preference matrix
//' and saves each sort as a binary file in the specified output directory. The output files
//' are named sequentially as `sort0.bin`, `sort1.bin`, and so on.
//'
//' @return This function returns the number of topological sorts.
//'
//' @export
// [[Rcpp::export]]
long long int precompute_topological_sorts(
   arma::umat prefs, int n_items, std::string output_directory, double save_frac) {
 if (!std::filesystem::exists(output_directory)) {
   if (!std::filesystem::create_directory(output_directory)) {
     Rcpp::stop("Failed to create directory " + output_directory);
   }
 }

 Graph g(n_items);
 for(size_t i{}; i < prefs.n_rows; i++) {
   g.addEdge(prefs.at(i, 0) - 1, prefs.at(i, 1) - 1);
 }

 long long int sort_count = 0;
 g.alltopologicalSort(output_directory, sort_count, save_frac);

 return sort_count;
}
