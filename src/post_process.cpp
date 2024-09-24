#include "post_process.h"

using namespace arma;

mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const ModelOptions& model_options
) {
  mat alpha_values(model_options.n_clusters, particle_vector.size());
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha_values.col(i) = particle_vector[i].parameters.alpha;
  }
  
  return alpha_values;
}

icube extract_rho_values(
    const std::vector<Particle>& particle_vector,
    const ModelOptions& model_options
) {
  icube rho_values(model_options.n_items, model_options.n_clusters, particle_vector.size());
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    rho_values.slice(i) = particle_vector[i].parameters.rho;
  }
  
  return rho_values;
}

mat extract_tau_values(
    const std::vector<Particle>& particle_vector,
    const ModelOptions& model_options
) {
  mat tau_values(model_options.n_clusters, particle_vector.size());
  
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
  arma::vec lli(particle_vector.size());
  std::transform(
    particle_vector.cbegin(), particle_vector.cend(), lli.begin(),
    [t](const Particle& p) { return p.log_likelihood_increment(t); }
  );
  double maxval = max(lli);
  log_marginal_likelihood = maxval + sum(exp(log(normalized_weights) + lli - maxval));
  
  return log_marginal_likelihood;
}

cube extract_cluster_probs(
  const std::vector<Particle>& particle_vector,
  const std::unique_ptr<Data>& data,
  const ModelOptions& model_options
) {
  cube cluster_probs(particle_vector.size(), data->n_users(), model_options.n_clusters);
  for(size_t i{}; i < particle_vector.size(); i++) {
    for(size_t j{}; j < data->n_users(); j++) {
      for(size_t k{}; k < model_options.n_clusters; k++) {
        cluster_probs(i, j, k) = particle_vector[i].particle_filters[0].cluster_probs(j, k);
      }
    }
  }
  return cluster_probs;
}