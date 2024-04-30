#include <memory>
#include "resampler.h"

using namespace arma;

ivec count_to_index(const vec& counts) {
  ivec indices(sum(counts));

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

ivec stratsys(vec probs, bool stratified) {
  size_t N = probs.size();
  vec u(N);
  vec rn = stratified ? randu(N) : vec(N, fill::value(randu()));

  for(size_t i{}; i < N; i++) u(i) = (i + rn(i)) / N;
  return digitize(cumsum(probs), u);
}

ivec Multinomial::resample(vec probs) {
  return Rcpp::sample(
    probs.size(), probs.size(), true,
    Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(probs)), false);
}

ivec Residual::resample(vec probs) {
  int N = probs.size();
  vec counts = floor(probs * N);
  int R = N - sum(counts);
  ivec result = count_to_index(counts);

  vec residual_weights = probs - counts / N;
  residual_weights = residual_weights / sum(residual_weights);
  ivec part2 = Rcpp::sample(
    N, R, true,
    Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(residual_weights)), false);

  return join_cols(result, part2);
}

ivec Stratified::resample(vec probs) {
  return stratsys(probs, true);
}

ivec Systematic::resample(vec probs) {
  return stratsys(probs, false);
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
