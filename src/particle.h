#pragma once
#include <RcppArmadillo.h>
#include <vector>
#include "prior.h"
#include "data.h"
#include "options.h"
#include "partition_functions.h"
#include "distances.h"
#include "resampler.h"

struct StaticParameters{
  StaticParameters() {}
  StaticParameters(const arma::vec& alpha, const arma::umat& rho, const arma::vec& tau);
  StaticParameters(const Prior& prior);
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
  arma::uvec index{};
  arma::mat cluster_probabilities{};
};

struct Particle{
  Particle() {}
  Particle(const Options& options, const StaticParameters& parameters,
           const std::unique_ptr<PartitionFunction>& pfun);
  ~Particle() = default;
  StaticParameters parameters;
  std::vector<ParticleFilter> particle_filters;
  double log_importance_weight{};
  arma::vec log_incremental_likelihood{};
  Rcpp::NumericVector log_normalized_particle_filter_weights;
  void run_particle_filter(
      unsigned int t, const Prior& prior, const std::unique_ptr<Data>& data,
      const std::unique_ptr<PartitionFunction>& pfun,
      const std::unique_ptr<Distance>& distfun,
      const std::unique_ptr<Resampler>& resampler,
      std::string latent_rank_proposal,
      bool conditional = false);
  bool rejuvenate(
    unsigned int T, const Options& options, const Prior& prior,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<Resampler>& resampler,
    const arma::vec& alpha_sd
  );
  int conditioned_particle_filter{};
  void sample_particle_filter();
  arma::vec logz{};
};

std::vector<Particle> create_particle_vector(const Options& options, const Prior& prior,
                                             const std::unique_ptr<PartitionFunction>& pfun);
std::vector<ParticleFilter> create_particle_filters(const Options& options);
arma::vec normalize_log_importance_weights(const std::vector<Particle>& particle_vector);
double log_marginal_likelihood_increment(
    const std::vector<Particle>& particle_vector,
    const arma::vec& normalized_log_importance_weights,
    int t
);
arma::vec compute_alpha_stddev(const std::vector<Particle>& particle_vector);
int find_unique_alphas(const std::vector<Particle>& particle_vector);

double compute_log_Z(const std::vector<ParticleFilter>& pf, int max_time);
