#pragma once
#include <RcppArmadillo.h>
#include <vector>
#include "prior.h"
#include "data.h"
#include "options.h"
#include "partition_functions.h"
#include "distances.h"

struct ParticleFilter{
  ParticleFilter() {}
  ~ParticleFilter() = default;
  arma::umat latent_rankings;
  arma::uvec cluster_assignments;
  double log_weight{};
};

struct Particle{
  Particle(const Options& options, const Prior& prior);
  ~Particle() = default;
  arma::vec alpha;
  arma::umat rho;
  arma::vec tau;
  std::vector<ParticleFilter> particle_filters;
  double log_importance_weight{};
  Rcpp::NumericVector log_normalized_particle_filter_weights;
  void run_particle_filter(
      unsigned int t, const Prior& prior, const std::unique_ptr<Data>& data,
      const std::unique_ptr<PartitionFunction>& pfun,
      const std::unique_ptr<Distance>& distfun);
};

std::vector<Particle> create_particle_vector(const Options& options, const Prior& prior);
std::vector<ParticleFilter> create_particle_filters(const Options& options);
