#include "post_process.h"

using namespace arma;

mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
) {
  mat alpha_values(prior.n_clusters, particle_vector.size());
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha_values.col(i) = particle_vector[i].parameters.alpha;
  }
  
  return alpha_values;
}

icube extract_rho_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
) {
  icube rho_values(prior.n_items, prior.n_clusters, particle_vector.size());
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    rho_values.slice(i) = particle_vector[i].parameters.rho;
  }
  
  return rho_values;
}

mat extract_tau_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
) {
  mat tau_values(prior.n_clusters, particle_vector.size());
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    tau_values.col(i) = particle_vector[i].parameters.tau;
  }
  
  return tau_values;
}

double extract_marginal_likelihood(
    const std::vector<Particle>& particle_vector,
    const arma::vec& normalized_weights,
    int t
) {
  double log_marginal_likelihood{};
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    log_marginal_likelihood += log(normalized_weights(i)) + 
      particle_vector[i].log_likelihood_increment(t);
  }
  
  return log_marginal_likelihood;
}