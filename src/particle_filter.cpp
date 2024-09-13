#include "particle_filter.h"

using namespace arma;

ParticleFilter::ParticleFilter(
  const std::unique_ptr<Data>& data,
  const Prior& prior
) : log_weight { zeros(data->n_timepoints()) }, 
cluster_labels { zeros<ivec>(data->n_users()) },
latent_rankings { zeros<imat>(0, prior.n_items)} {}

std::vector<ParticleFilter> create_particle_filter_vectors(
    const SMCOptions& smc_options,
    const std::unique_ptr<Data>& data,
    const Prior& prior
) {
  return std::vector<ParticleFilter>(
    smc_options.n_particle_filters, 
    ParticleFilter(data, prior)
    );
}
