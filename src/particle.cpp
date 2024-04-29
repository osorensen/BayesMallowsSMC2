#include <algorithm>
#include "particle.h"
using namespace arma;

Particle::Particle(const Options& options, const Prior& prior) :
  alpha (randg(prior.n_clusters, distr_param(prior.alpha_shape, 1 / prior.alpha_rate))),
  rho (umat(prior.n_items, prior.n_clusters)),
  tau (normalise(randg(prior.n_clusters, distr_param(prior.cluster_concentration, 1)), 1)),
  particle_filters(create_particle_filters(options)),
  log_normalized_particle_filter_weights (
      Rcpp::NumericVector(options.n_particle_filters, -log(options.n_particle_filters))
  )
  {
    rho.each_col([&prior](uvec& a){ a = shuffle(regspace<uvec>(1, prior.n_items)); });
  }

void Particle::run_particle_filter(
    unsigned int t, const Prior& prior, const std::unique_ptr<Data>& data,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun) {
  // resample
  if(t > 0) {
    Rcpp::NumericVector probs = exp(log_normalized_particle_filter_weights);
    ivec new_inds = Rcpp::sample(probs.size(), probs.size(), true, probs, false);
    std::vector<ParticleFilter> tmp(particle_filters.size());
    for(size_t i{}; i < new_inds.size(); i++) {
      tmp[i] = particle_filters[new_inds[i]];
    }
    particle_filters = tmp;
    std::fill(log_normalized_particle_filter_weights.begin(),
              log_normalized_particle_filter_weights.end(), -log(particle_filters.size()));
  }

  // sample latent rankings
  for(auto& pf : particle_filters) {
    auto proposal = data->sample_latent_rankings(t, prior);
    pf.latent_rankings = proposal.proposal;
    vec log_cluster_contribution(prior.n_clusters);

    for(size_t c{}; c < prior.n_clusters; c++) {
      unsigned int total_distance{};
      for(size_t i{}; i < pf.latent_rankings.n_cols; i++) {
        total_distance += distfun->d(pf.latent_rankings.col(i), rho.col(c));
      }
      log_cluster_contribution(c) = log(tau(c)) - pfun->logz(alpha(c)) -
        alpha(c) / prior.n_items * total_distance;
    }
    double maxval = log_cluster_contribution.max();
    double log_prob = maxval + log(accu(exp(log_cluster_contribution - maxval)));
    pf.log_weight = log_prob - proposal.log_probability;
  }
  Rcpp::NumericVector tmp_pf_weights(log_normalized_particle_filter_weights.size());
  std::transform(
    particle_filters.cbegin(), particle_filters.cend(), tmp_pf_weights.begin(),
    [](const ParticleFilter& pf){ return pf.log_weight; });
  double maxval = Rcpp::max(tmp_pf_weights);

  log_incremental_likelihood.resize(log_incremental_likelihood.size() + 1);
  log_incremental_likelihood(log_incremental_likelihood.size() - 1) =
    maxval + log(mean(exp(tmp_pf_weights - maxval)));

  log_normalized_particle_filter_weights =
    tmp_pf_weights - (maxval + log(sum(exp(tmp_pf_weights - maxval))));
}

std::vector<Particle> create_particle_vector(const Options& options, const Prior& prior) {
  std::vector<Particle> result;
  result.reserve(options.n_particles);

  for(size_t i{}; i < options.n_particles; i++) {
    result.push_back(Particle{options, prior});
  }

  return result;
}

std::vector<ParticleFilter> create_particle_filters(const Options& options) {
  std::vector<ParticleFilter> result;
  result.reserve(options.n_particle_filters);
  for(size_t i{}; i < options.n_particle_filters; i++) {
    result.push_back(ParticleFilter{});
  }

  return result;
}
