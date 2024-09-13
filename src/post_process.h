#pragma once
#include <RcppArmadillo.h>

#include "particle.h"

arma::mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
);

arma::icube extract_rho_values(
  const std::vector<Particle>& particle_vector,
  const Prior& prior
);

arma::mat extract_tau_values(
  const std::vector<Particle>& particle_vector,
  const Prior& prior
);

double extract_marginal_likelihood(
  const std::vector<Particle>& particle_vector,
  const arma::vec& normalized_weights,
  int t
);