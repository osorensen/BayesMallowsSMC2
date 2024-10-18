#include <RcppArmadillo.h>
using namespace arma;

// [[Rcpp::export]]
ivec resample_counts(unsigned int size, vec& probs) {
  unsigned int N = probs.size();
  ivec outcome = zeros<ivec>(N);
  R::rmultinom(size, probs.begin(), N, outcome.begin());
  return outcome;
}

// [[Rcpp::export]]
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
