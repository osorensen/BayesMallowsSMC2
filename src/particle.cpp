#include "particle.h"
using namespace arma;
Particle::Particle(const Prior& prior) :
  alpha (randg(prior.n_clusters, distr_param(prior.alpha_shape, 1 / prior.alpha_rate))),
  rho (umat(prior.n_items, prior.n_clusters)),
  tau (normalise(randg(prior.n_clusters, distr_param(prior.cluster_concentration, 1)), 1))
  {
    rho.each_col([&prior](uvec& a){ a = shuffle(regspace<uvec>(1, prior.n_items)); });
  }

