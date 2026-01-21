#include <memory>
#include "resampler.h"

using namespace arma;

ivec count_between_intervals(const vec& cumprob, const vec& u) {
  ivec counts(cumprob.size());
  size_t last_index{};

  for (size_t i{}; i < cumprob.size(); ++i) {
    uvec indices = find(u < cumprob(i), 1, "last");
    size_t current_index{};
    if(!indices.is_empty()) {
      current_index = indices(0) + 1;
    } else {
      current_index = last_index;
    }

    counts(i) = current_index - last_index;
    last_index = current_index;
  }

  return counts;
}


ivec stratsys(int n_samples, vec probs, bool stratified) {
  vec u(n_samples);
  vec rn = stratified ? Rcpp::runif(n_samples) : vec(n_samples, fill::value(R::runif(0, 1)));

  for(size_t i{}; i < n_samples; i++) u(i) = (i + rn(i)) / n_samples;
  return count_between_intervals(cumsum(probs), u);
}

ivec resample_counts(unsigned int size, vec& probs) {
  unsigned int N = probs.size();
  ivec outcome = zeros<ivec>(N);
  R::rmultinom(size, probs.begin(), N, outcome.begin());
  return outcome;
}

ivec Multinomial::resample(int n_samples, vec probs) {
  return resample_counts(n_samples, probs);
}

ivec Residual::resample(int n_samples, vec probs) {
  ivec counts = conv_to<ivec>::from(floor(n_samples * probs));
  double R = sum(counts);
  if(n_samples > R) {
    vec new_probs = (n_samples * probs - counts) / (n_samples - R);
    ivec part2 = resample_counts(n_samples - R, new_probs);
    counts += part2;
  }
  return counts;
}

ivec Stratified::resample(int n_samples, vec probs) {
  return stratsys(n_samples, probs, true);
}

ivec Systematic::resample(int n_samples, vec probs) {
  return stratsys(n_samples, probs, false);
}

std::unique_ptr<Resampler> choose_resampler(std::string resampler) {
  if(resampler == "multinomial") {
    return std::make_unique<Multinomial>();
  } else if(resampler == "residual") {
    return std::make_unique<Residual>();
  } else if(resampler == "stratified") {
    return std::make_unique<Stratified>();
  } else if(resampler == "systematic") {
    return std::make_unique<Systematic>();
  } else {
    Rcpp::stop("Unknown resampler.");
  }
}
