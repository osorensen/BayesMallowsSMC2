#pragma once
#include <RcppArmadillo.h>

#include "particle.h"

arma::mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
);

arma::cube extract_rho_values(
  const std::vector<Particle>& particle_vector,
  const Prior& prior
);