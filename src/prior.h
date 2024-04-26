#pragma once
#include <Rcpp.h>

struct Prior{
  Prior(const Rcpp::List& input_prior);
  ~Prior() = default;
  double alpha_shape;
  double alpha_rate;
  int cluster_concentration;
  int n_clusters;
  int n_items;
};
