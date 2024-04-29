#pragma once

#include <RcppArmadillo.h>
#include "particle.h"

arma::uvec sample_cluster_assignments(
  const arma::umat& latent_rankings, const StaticParameters& parameters,
  const std::unique_ptr<PartitionFunction>& pfun,
  const std::unique_ptr<Distance>& distfun
);
