#pragma once
#include <RcppArmadillo.h>
#include <string>

struct ModelOptions{
  ModelOptions(
    const Rcpp::List& input_model_options,
    const Rcpp::List& input_data);
  ~ModelOptions() = default;
  std::string metric;
  int n_clusters;
  int n_items;
};