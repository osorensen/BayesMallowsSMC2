#pragma once
#include <RcppArmadillo.h>

struct ModelOptions{
  ModelOptions(const Rcpp::List& input_model_options);
  ~ModelOptions() = default;
  std::string metric;
};