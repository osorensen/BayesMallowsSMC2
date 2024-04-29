#pragma once
#include <RcppArmadillo.h>
#include <vector>
#include "prior.h"
#include "data.h"
#include "options.h"
#include "partition_functions.h"
#include "distances.h"

struct StaticParameters{
  arma::vec alpha;
  arma::umat rho;
  arma::vec tau;
};

struct ParticleFilter{
  ParticleFilter() {}
  ~ParticleFilter() = default;
  arma::umat latent_rankings{};
  arma::uvec cluster_assignments{};
  arma::vec log_weight{};
};

struct Particle{
  Particle(const Options& options, const Prior& prior);
  ~Particle() = default;
  StaticParameters parameters;
  std::vector<ParticleFilter> particle_filters;
  double log_importance_weight{};
  arma::vec log_incremental_likelihood{};
  Rcpp::NumericVector log_normalized_particle_filter_weights;
  void run_particle_filter(
      unsigned int t, const Prior& prior, const std::unique_ptr<Data>& data,
      const std::unique_ptr<PartitionFunction>& pfun,
      const std::unique_ptr<Distance>& distfun);
  bool rejuvenate(
    unsigned int T, const Options& options, const Prior& prior,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun,
    const arma::vec& alpha_sd
  );
};

std::vector<Particle> create_particle_vector(const Options& options, const Prior& prior);
std::vector<ParticleFilter> create_particle_filters(const Options& options);
