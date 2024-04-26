#pragma once
#include <RcppArmadillo.h>

struct Options{
  Options(const Rcpp::List& input_options);
  ~Options() = default;

  unsigned int n_particles;
  unsigned int n_particle_filters;
};
