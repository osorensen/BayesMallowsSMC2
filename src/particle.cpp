#include "leap_and_shift.h"
#include "particle.h"

using namespace arma;

Particle::Particle(
  const Prior& prior,
  const SMCOptions& smc_options,
  const std::unique_ptr<Data>& data
) : 
  log_weight { zeros(data->n_timepoints())},
  log_likelihood_increment { zeros(data->n_timepoints())},
  particle_filters { create_particle_filter_vectors(smc_options, data) } {
  parameters.alpha = randg(
    prior.n_clusters, 
    distr_param(prior.alpha_shape, 1 / prior.alpha_rate)
    );
  parameters.rho = imat(prior.n_items, prior.n_clusters);
  for(size_t i{}; i < prior.n_clusters; i++) {
    parameters.rho.col(i) = shuffle(regspace<ivec>(1, prior.n_items));
  }
  parameters.tau = randg(
    prior.n_clusters, 
    distr_param(prior.cluster_concentration, 1)
    );
  parameters.tau = normalise(parameters.tau, 1);
}

Particle::Particle(
  const Prior& prior,
  const SMCOptions& smc_options,
  const std::unique_ptr<Data>& data,
  const AlphaSummaries& alpha_summaries,
  const StaticParameters& static_parameters
) : 
  log_weight { zeros(data->n_timepoints())},
  log_likelihood_increment { zeros(data->n_timepoints())},
  particle_filters { create_particle_filter_vectors(smc_options, data) }
  {
    parameters.alpha = static_parameters.alpha;
    parameters.rho = static_parameters.rho;
    for(size_t c{}; c < prior.n_clusters; c++) {
      parameters.alpha(c) = R::rlnorm(
        log(parameters.alpha(c)), std::max(.001, alpha_summaries.alpha_sd(c)));
      
      parameters.rho.col(c) = 
        leap_and_shift(parameters.rho.col(c), prior);
    }
    
    parameters.tau = static_parameters.tau;
  }

std::vector<Particle> create_particle_vector(
    const Prior& prior,
    const SMCOptions& smc_options,
    const std::unique_ptr<Data>& data
) {
  std::vector<Particle> result;
  result.reserve(smc_options.n_particles);
  
  for(size_t i{}; i < smc_options.n_particles; i++) {
    result.push_back(Particle{prior, smc_options, data});
  }
  
  return result;
}

mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
) {
  mat alpha_values(particle_vector.size(), prior.n_clusters);
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha_values.row(i) = particle_vector[i].parameters.alpha.t();
  }
  
  return alpha_values;
}

int count_unique_rows(const arma::mat& matrix) {
  std::set<std::vector<double>> unique_rows;
  
  for (size_t i = 0; i < matrix.n_rows; ++i) {
    std::vector<double> row = arma::conv_to<std::vector<double>>::from(matrix.row(i));
    unique_rows.insert(row);
  }
  
  return unique_rows.size();
}

AlphaSummaries compute_alpha_summaries(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
) {
  mat alpha_values = extract_alpha_values(particle_vector, prior);
  return AlphaSummaries{mean(alpha_values, 0), stddev(alpha_values, 0, 0)};
}