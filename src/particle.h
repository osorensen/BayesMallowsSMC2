#pragma once
#include <RcppArmadillo.h>
#include <vector>
#include "prior.h"
#include "data.h"
#include "options.h"

struct ParticleFilter{
  ParticleFilter(const Data& dat);
  ~ParticleFilter() = default;
  arma::umat latent_rankings;
  arma::uvec cluster_assignments;
};

struct Particle{
  Particle(const Prior& prior);
  ~Particle() = default;
  arma::vec alpha;
  arma::umat rho;
  arma::vec tau;
};

std::vector<Particle> create_particle_vector(
  const Options& options, const Prior& prior, const std::unique_ptr<Data>& dat
);
