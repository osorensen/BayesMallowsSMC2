#pragma once
#include <RcppArmadillo.h>

#include "data.h"
#include "distances.h"
#include "particle_filter.h"
#include "partition_functions.h"
#include "prior.h"
#include "smc_options.h"

struct StaticParameters{
  arma::vec alpha;
  arma::imat rho;
  arma::vec tau;
};

struct AlphaSummaries{
  arma::vec alpha_mean;
  arma::vec alpha_sd;
};

struct Particle{
  Particle(
    const Prior& prior,
    const SMCOptions& smc_options,
    const std::unique_ptr<Data>& data
  );
  Particle(
    const Prior& prior,
    const SMCOptions& smc_options, 
    const std::unique_ptr<Data>& data,
    const AlphaSummaries& alpha_summaries,
    const StaticParameters& static_parameters
  );
  ~Particle() = default;
  StaticParameters parameters;
  
  arma::vec log_weight{};
  arma::vec log_likelihood_increment{};
  std::vector<ParticleFilter> particle_filters;
  
  void run_particle_filter(
      int t,
      const std::unique_ptr<Data>& data,
      const std::unique_ptr<Distance>& distfun,
      const std::unique_ptr<PartitionFunction>& pfun
  );
  bool rejuvenate(
    int t,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<PartitionFunction>& pfun,
    const Prior& prior,
    const SMCOptions& smc_options,
    const AlphaSummaries& alpha_summaries
  );
};

std::vector<Particle> create_particle_vector(
    const Prior& prior,
    const SMCOptions& smc_options,
    const std::unique_ptr<Data>& data
);

int count_unique_cols(const arma::mat& matrix);

AlphaSummaries compute_alpha_summaries(
  const std::vector<Particle>& particle_vector,
  const Prior& prior
);