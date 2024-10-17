#include <memory>
#include "resampler.h"

using namespace arma;

ivec count_to_index(const vec& counts, double R) {
  ivec indices(R);

  int idx = 0;
  for (uword i = 0; i < counts.n_elem; ++i) {
    for (int j = 0; j < counts(i); ++j) {
      indices(idx++) = i;
    }
  }
  return indices;
}

ivec digitize(const vec& bins, const vec& values) {
  ivec indices(values.n_elem);

  for (uword i = 0; i < values.n_elem; ++i) {
    double val = values(i);
    uword index = 0;
    while (index < bins.n_elem && val >= bins(index)) {
      ++index;
    }
    indices(i) = index;
  }
  return indices;
}

ivec stratsys(int n_samples, vec probs, bool stratified) {
  vec u(n_samples);
  vec rn = stratified ? randu(n_samples) : vec(n_samples, fill::value(randu()));

  for(size_t i{}; i < n_samples; i++) u(i) = (i + rn(i)) / n_samples;
  return digitize(cumsum(probs), u);
}

ivec Multinomial::resample(int n_samples, vec probs) {
  return Rcpp::sample(
    probs.size(), n_samples, true,
    Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(probs)), false);
}

ivec Residual::resample(int n_samples, vec probs) {
  vec counts = floor(n_samples * probs);
  double R = sum(counts);
  ivec part2;
  if(n_samples > R) {
    vec new_probs = (n_samples * probs - counts) / (n_samples - R);
    part2 = Rcpp::sample(
      new_probs.size(), n_samples - R, true,
      Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(new_probs)), false);
  }
  ivec part1 = count_to_index(counts, R);
  return join_cols(part1, part2);
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
