#include "leap_and_shift.h"
#include "model_options.h"
#include "particle.h"
#include "post_process.h"

using namespace arma;

Particle::Particle(
  const Prior& prior,
  const SMCOptions& smc_options,
  const ModelOptions& model_options,
  const std::unique_ptr<Data>& data
) : 
  log_weight { zeros(data->n_timepoints())},
  log_likelihood_increment { zeros(data->n_timepoints())},
  particle_filters { create_particle_filter_vectors(smc_options, model_options, data) } {
  alpha = randg(distr_param(prior.alpha_shape, 1 / prior.alpha_rate));
  rho = shuffle(regspace<ivec>(1, model_options.n_items));
}

std::vector<Particle> create_particle_vector(
    const Prior& prior,
    const SMCOptions& smc_options,
    const ModelOptions& model_options,
    const std::unique_ptr<Data>& data
) {
  std::vector<Particle> result;
  result.reserve(smc_options.n_particles);
  
  for(size_t i{}; i < smc_options.n_particles; i++) {
    result.push_back(Particle{prior, smc_options, model_options, data});
  }
  
  return result;
}

int count_unique_cols(const arma::mat& matrix) {
  std::set<std::vector<double>> unique_cols;
  
  for (size_t i = 0; i < matrix.n_cols; ++i) {
    std::vector<double> col = arma::conv_to<std::vector<double>>::from(matrix.col(i));
    unique_cols.insert(col);
  }
  
  return unique_cols.size();
}

AlphaSummaries compute_alpha_summaries(
    const std::vector<Particle>& particle_vector,
    const ModelOptions& model_options
) {
  vec alpha_values = extract_alpha_values(particle_vector, model_options);
  return AlphaSummaries{mean(alpha_values), stddev(alpha_values)};
}