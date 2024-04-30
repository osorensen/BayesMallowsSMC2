// Based on https://www.geeksforgeeks.org/all-topological-sorts-of-a-directed-acyclic-graph/
// Originally written by Utkarsh Trivedi

// [[Rcpp::depends(RcppArmadillo)]]

#include <RcppArmadillo.h>
#include <vector>
#include <list>
#include "typedefs.h"
using namespace std;

class Graph {
  int n_items;
  list<int> *adj;
  vector<int> indegree;
  void alltopologicalSortUtil(vector<int>& res, vector<bool>& visited);

public:
  Graph(int n_items);
  void addEdge(int v, int w);
  void alltopologicalSort();
  vector<vector<int>> m;
};

Graph::Graph(int n_items) : n_items { n_items } {
  adj = new list<int>[n_items];
  for (int i = 0; i < n_items; i++) indegree.push_back(0);
}

void Graph::addEdge(int v, int w) {
  adj[v].push_back(w);
  indegree[w]++;
}

void Graph::alltopologicalSortUtil(vector<int>& res, vector<bool>& visited) {
  bool flag = false;
  Rcpp::IntegerVector visit_order = Rcpp::sample(n_items, n_items) - 1;

  for (int i : visit_order) {
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
  vector<bool> visited;
  visited.resize(n_items);
  fill(visited.begin(), visited.end(), false);
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
