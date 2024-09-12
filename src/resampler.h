#pragma once
#include <RcppArmadillo.h>

#include "smc_options.h"

struct Resampler {
  Resampler() {};
  virtual ~Resampler() = default;
  virtual arma::ivec resample(int n_samples, arma::vec probs) = 0;
};

struct Multinomial : Resampler {
  arma::ivec resample(int n_samples, arma::vec probs) override;
};

struct Residual : Resampler {
  arma::ivec resample(int n_samples, arma::vec probs) override;
};

struct Stratified : Resampler {
  arma::ivec resample(int n_samples, arma::vec probs) override;
};

struct Systematic : Resampler {
  arma::ivec resample(int n_samples, arma::vec probs) override;
};

std::unique_ptr<Resampler> choose_resampler(const SMCOptions& smc_options);

template<typename T>
std::vector<T> replace_elements(
    const std::vector<T>& incoming_vector, 
    const arma::ivec& new_inds
) {
  std::vector<T> new_values;
  new_values.reserve(incoming_vector.size());
  
  for (size_t i = 0; i < new_inds.n_elem; ++i) {
    new_values.push_back(incoming_vector[new_inds[i]]);
  }
  
  return new_values;
}