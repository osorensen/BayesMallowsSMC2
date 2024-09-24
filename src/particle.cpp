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
  parameters.alpha = randg(
    model_options.n_clusters, 
    distr_param(prior.alpha_shape, 1 / prior.alpha_rate)
    );
  parameters.rho = imat(model_options.n_items, model_options.n_clusters);
  for(size_t i{}; i < model_options.n_clusters; i++) {
    parameters.rho.col(i) = shuffle(regspace<ivec>(1, model_options.n_items));
  }
  parameters.tau = randg(
    model_options.n_clusters, 
    distr_param(prior.cluster_concentration, 1)
    );
  parameters.tau = normalise(parameters.tau, 1);
}

Particle::Particle(
  const SMCOptions& smc_options,
  const ModelOptions& model_options,
  const std::unique_ptr<Data>& data,
  const AlphaSummaries& alpha_summaries,
  const StaticParameters& static_parameters
) : 
  log_weight { zeros(data->n_timepoints())},
  log_likelihood_increment { zeros(data->n_timepoints())},
  particle_filters { create_particle_filter_vectors(smc_options, model_options, data) }
  {
    parameters.alpha = static_parameters.alpha;
    parameters.rho = static_parameters.rho;
    for(size_t c{}; c < model_options.n_clusters; c++) {
      parameters.alpha(c) = R::rlnorm(
        log(parameters.alpha(c)), std::max(.001, alpha_summaries.alpha_sd(c)));
      
      parameters.rho.col(c) = leap_and_shift(parameters.rho.col(c));
    }
    
    parameters.tau = static_parameters.tau;
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

LogClusterContribution Particle::compute_log_likelihood_contribution(
    const ivec& ranking,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<PartitionFunction>& pfun
) {
  vec log_cluster_contribution = zeros(parameters.alpha.size());
  
  for(size_t c{}; c < log_cluster_contribution.size(); c++) {
    log_cluster_contribution(c) = log(parameters.tau(c)) -
      pfun->logz(parameters.alpha(c)) - parameters.alpha(c) *
      distfun->d(ranking, parameters.rho.col(c));
  }
  double max_log = log_cluster_contribution.max();
  
  return LogClusterContribution{
    log_cluster_contribution,
    max_log + log(sum(exp(log_cluster_contribution - max_log)))
  };
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
  mat alpha_values = extract_alpha_values(particle_vector, model_options);
  return AlphaSummaries{mean(alpha_values, 1), stddev(alpha_values, 0, 1)};
}