#include "particle_filter.h"

using namespace arma;

ParticleFilter::ParticleFilter(
  const std::unique_ptr<Data>& data,
  const Prior& prior
) : log_weight { zeros(data->n_timepoints()) }, 
cluster_labels { zeros<ivec>(data->n_users()) },
latent_rankings { zeros<imat>(0, prior.n_items + 1)} {}

ivec ParticleFilter::extract_latent_ranking(int u) {
  uvec index = find(latent_rankings.col(0) == u);
  imat ret = latent_rankings.rows(index);
  ret = ret.cols(1, ret.n_cols - 1);
  
  return ret.t();
}

void ParticleFilter::insert_latent_ranking(const ivec& r, int u) {
  uvec index = find(latent_rankings.col(0) == u);
  latent_rankings.row(as_scalar(index)) = join_horiz(ivec{u}, r.t());
}

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

bool check_rank_consistency(
    const ivec& latent, 
    const ivec& observed,
    const uvec& observed_inds
) {
  return approx_equal(
    latent(observed_inds), observed(observed_inds), "absdiff", 0);
}