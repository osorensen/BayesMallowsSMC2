#pragma once
#include <RcppArmadillo.h>

#include "data.h"
#include "smc_options.h"

struct ParticleFilter{
  ParticleFilter(const std::unique_ptr<Data>& data);
  arma::vec log_weight{};
  arma::ivec cluster_labels{};
};

std::vector<ParticleFilter> create_particle_filter_vectors(
  const SMCOptions& smc_options,
  const std::unique_ptr<Data>& data
);
