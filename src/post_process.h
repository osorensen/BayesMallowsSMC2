#pragma once
#include <RcppArmadillo.h>

#include "particle.h"

arma::mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const ModelOptions& model_options
);

arma::icube extract_rho_values(
  const std::vector<Particle>& particle_vector,
  const ModelOptions& model_options
);

arma::mat extract_tau_values(
  const std::vector<Particle>& particle_vector,
  const ModelOptions& model_options
);

double extract_marginal_likelihood(
  const std::vector<Particle>& particle_vector,
  const arma::vec& normalized_weights,
  int t
);

arma::cube extract_cluster_probs(
    const std::vector<Particle>& particle_vector,
    const std::unique_ptr<Data>& data,
    const ModelOptions& model_options
);