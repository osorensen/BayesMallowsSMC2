#include <algorithm>
#include "particle.h"
using namespace arma;

Particle::Particle(const Options& options, const Prior& prior) :
  alpha (randg(prior.n_clusters, distr_param(prior.alpha_shape, 1 / prior.alpha_rate))),
  rho (umat(prior.n_items, prior.n_clusters)),
  tau (normalise(randg(prior.n_clusters, distr_param(prior.cluster_concentration, 1)), 1)),
  particle_filters(create_particle_filters(options)),
  log_normalized_particle_filter_weights {zeros(options.n_particle_filters)}
  {
    rho.each_col([&prior](uvec& a){ a = shuffle(regspace<uvec>(1, prior.n_items)); });
  }

void Particle::run_particle_filter(unsigned int t) {
  if(t > 0) {

  }

  // sample latent ranks

  // sample cluster assignments

  // compute weights
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
