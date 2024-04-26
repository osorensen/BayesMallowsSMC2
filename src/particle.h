#pragma once
#include <RcppArmadillo.h>
#include "prior.h"

struct Particle{
  Particle(const Prior& prior);
  ~Particle() = default;
  arma::vec alpha;
  arma::umat rho;
  arma::vec tau;
};
