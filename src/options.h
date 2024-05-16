#pragma once
#include <RcppArmadillo.h>
#include <string>

struct Options{
  Options(const Rcpp::List& input_options);
  ~Options() = default;

  const std::string metric;
  const std::string resampler;
  unsigned int n_particles;
  unsigned int n_particle_filters;
  unsigned int max_particle_filters;
  unsigned int resampling_threshold;
  const bool verbose;
  const bool trace;
};
