#pragma once
#include <RcppArmadillo.h>
#include <vector>

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

std::unique_ptr<Resampler> choose_resampler(std::string resampler);

template <typename T>
std::vector<T> update_vector(
    const arma::ivec& new_inds, const std::vector<T>& particle_vector)  {
  std::vector<T> tmp(particle_vector.size());
  std::transform(new_inds.begin(), new_inds.end(), tmp.begin(),
                 [&](int index) { return particle_vector[index]; });
  return tmp;
}
