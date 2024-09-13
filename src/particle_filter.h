#pragma once
#include <RcppArmadillo.h>

#include "data.h"
#include "prior.h"
#include "smc_options.h"

struct ParticleFilter{
  ParticleFilter(
    const std::unique_ptr<Data>& data,
    const Prior& prior
  );
  arma::vec log_weight{};
  arma::ivec cluster_labels{};
  arma::imat latent_rankings{};
};

std::vector<ParticleFilter> create_particle_filter_vectors(
  const SMCOptions& smc_options,
  const std::unique_ptr<Data>& data,
  const Prior& prior
);
