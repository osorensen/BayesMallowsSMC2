#pragma once
#include <Rcpp.h>

struct Prior{
  Prior(
    const Rcpp::List& input_prior,
    const Rcpp::List& input_data
  );
  ~Prior() = default;
  double alpha_shape;
  double alpha_rate;
  int cluster_concentration;
};