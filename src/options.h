#pragma once
#include <RcppArmadillo.h>
#include <string>

struct Options{
  Options(const Rcpp::List& input_options);
  ~Options() = default;

  const std::string metric;
  unsigned int n_particles;
  unsigned int n_particle_filters;
};
