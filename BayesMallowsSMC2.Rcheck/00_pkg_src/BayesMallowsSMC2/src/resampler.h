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
std::vector<T> update_vector(const arma::ivec& counts, const std::vector<T>& particle_vector) {
  size_t total_size = sum(counts);
  std::vector<T> result(total_size);

  T* result_ptr = result.data();

  for (size_t i = 0; i < counts.size(); ++i) {
    int count = counts[i];
    if (count > 0) {
      std::fill_n(result_ptr, count, particle_vector[i]);
      result_ptr += count;
    }
  }

  return result;
}
