// Based on https://www.geeksforgeeks.org/all-topological-sorts-of-a-directed-acyclic-graph/
// Originally written by Utkarsh Trivedi

// [[Rcpp::depends(RcppArmadillo)]]

#include <RcppArmadillo.h>
#include <vector>
#include <list>
#include <string>
#include <sstream>
#include <filesystem>
#include "typedefs.h"
using namespace std;

class Graph {
  int n_items;
  std::vector<std::list<int>> adj;
  vector<int> indegree;
  void alltopologicalSortUtil(vector<int>& res, vector<bool>& visited);

public:
  Graph(int n_items);
  void addEdge(int v, int w);
  void alltopologicalSort();
  vector<vector<int>> m;
};

Graph::Graph(int n_items) : n_items { n_items }, adj(n_items),
indegree(n_items, 0) {}

void Graph::addEdge(int v, int w) {
  adj[v].push_back(w);
  indegree[w]++;
}

void Graph::alltopologicalSortUtil(vector<int>& res, vector<bool>& visited) {
  bool flag = false;

  for (size_t i{}; i < n_items; i++) {
    if (indegree[i] == 0 && !visited[i]) {
      list<int>:: iterator j;
      for (j = adj[i].begin(); j != adj[i].end(); j++)
        indegree[*j]--;

      res.push_back(i);
      visited[i] = true;
      alltopologicalSortUtil(res, visited);

      visited[i] = false;
      res.erase(res.end() - 1);
      for (j = adj[i].begin(); j != adj[i].end(); j++)
        indegree[*j]++;

      flag = true;
    }
  }

  if (!flag){
    m.push_back(res);
  }
}

void Graph::alltopologicalSort() {
  vector<bool> visited(n_items, false);
  vector<int> res;
  alltopologicalSortUtil(res, visited);
}

topological_sorts_user all_topological_sorts(const arma::umat& prefs, int n_items) {
  Graph g(n_items);
  for(size_t i{}; i < prefs.n_rows; i++) {
    g.addEdge(prefs.at(i, 0) - 1, prefs.at(i, 1) - 1);
  }
  g.alltopologicalSort();

  topological_sorts_user ret;
  ret.reserve(g.m.size());
  for(size_t i{}; i < g.m.size(); i++) {
    ret.push_back(arma::ivec(g.m[i]) + 1);
  }

  return ret;
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
//' @param max_files_to_save An integer specifying an upper limit on the number of files to save. If
//'  the number of topological sorts exceeds `max_files_to_save`, a random sample of size `max_files_to_save`
//'  is taken at random, and the corresponding sorts saved.
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
int precompute_topological_sorts(
    arma::umat prefs, int n_items, std::string output_directory,
    int max_files_to_save) {
 if (!std::filesystem::exists(output_directory)) {
   if (!std::filesystem::create_directory(output_directory)) {
     Rcpp::stop("Failed to create directory " + output_directory);
   }
 }

 auto sorts = all_topological_sorts(prefs, n_items);
 int n_sorts = sorts.size();
 topological_sorts_user temp_sorts;

 if(sorts.size() > max_files_to_save) {
   Rcpp::IntegerVector inds_to_keep = Rcpp::sample(sorts.size(), max_files_to_save);
   for(auto i : inds_to_keep) {
     temp_sorts.push_back(sorts[i]);
   }
   sorts = std::move(temp_sorts);
 }

 for (size_t i = 0; i < sorts.size(); i++) {
   std::ostringstream filename_stream;
   filename_stream << output_directory << "/sort" << i << ".bin";
   std::string filename = filename_stream.str();
   sorts[i].save(filename);
 }
 return n_sorts;
}
