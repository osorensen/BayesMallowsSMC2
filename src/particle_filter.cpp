#include "particle_filter.h"

using namespace arma;

ParticleFilter::ParticleFilter(
  const std::unique_ptr<Data>& data
) : log_weight { zeros(data->n_timepoints()) } {}

std::vector<ParticleFilter> create_particle_filter_vectors(
    const SMCOptions& smc_options,
    const std::unique_ptr<Data>& data
) {
  return std::vector<ParticleFilter>(
    smc_options.n_particle_filters, 
    ParticleFilter(data)
    );
}
