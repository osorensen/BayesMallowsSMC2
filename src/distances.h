#pragma once
#include <RcppArmadillo.h>

#include "model_options.h"

struct Distance {
  Distance() {};
  virtual ~Distance() = default;
  virtual int d(const arma::ivec& r1, const arma::ivec& r2) = 0;
  int d(const arma::imat& m1, const arma::ivec& m2);
};

std::unique_ptr<Distance> choose_distance_function(
    const ModelOptions& model_options);

struct CayleyDistance : Distance {
  int d(const arma::ivec& r1, const arma::ivec& r2) override;
};

struct FootruleDistance : Distance {
  int d(const arma::ivec& r1, const arma::ivec& r2) override;
};

struct HammingDistance : Distance {
  int d(const arma::ivec& r1, const arma::ivec& r2) override;
};

struct KendallDistance : Distance {
  int d(const arma::ivec& r1, const arma::ivec& r2) override;
};

struct SpearmanDistance : Distance {
  int d(const arma::ivec& r1, const arma::ivec& r2) override;
};

struct UlamDistance : Distance {
  int d(const arma::ivec& r1, const arma::ivec& r2) override;
};