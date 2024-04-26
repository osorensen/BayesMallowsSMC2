#pragma once
#include <memory>
#include <RcppArmadillo.h>

struct Distance {
  Distance() {};
  virtual ~Distance() = default;
  virtual unsigned int d(const arma::uvec& r1, const arma::uvec& r2) = 0;
};

std::unique_ptr<Distance> choose_distance_function(const std::string& metric);

struct CayleyDistance : Distance {
  unsigned int d(const arma::uvec& r1, const arma::uvec& r2) override;
};

struct FootruleDistance : Distance {
  unsigned int d(const arma::uvec& r1, const arma::uvec& r2) override;
};

struct HammingDistance : Distance {
  unsigned int d(const arma::uvec& r1, const arma::uvec& r2) override;
};

struct KendallDistance : Distance {
  unsigned int d(const arma::uvec& r1, const arma::uvec& r2) override;
};

struct SpearmanDistance : Distance {
  unsigned int d(const arma::uvec& r1, const arma::uvec& r2) override;
};

struct UlamDistance : Distance {
  unsigned int d(const arma::uvec& r1, const arma::uvec& r2) override;
};
