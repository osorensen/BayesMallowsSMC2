#include "distances.h"
using namespace arma;

std::unique_ptr<Distance> choose_distance_function(
    const ModelOptions& model_options) {
  if(model_options.metric == "cayley") {
    return std::make_unique<CayleyDistance>();
  } else if(model_options.metric == "footrule") {
    return std::make_unique<FootruleDistance>();
  } else if(model_options.metric == "hamming") {
    return std::make_unique<HammingDistance>();
  } else if(model_options.metric == "kendall") {
    return std::make_unique<KendallDistance>();
  } else if(model_options.metric == "spearman") {
    return std::make_unique<SpearmanDistance>();
  } else if(model_options.metric == "ulam") {
    return std::make_unique<UlamDistance>();
  } else {
    Rcpp::stop("Unknown metric.");
  }
}

int Distance::d(const imat& m1, const ivec& r2) {
  int total_distance = 0;
  
  if(m1.n_cols != r2.size()) {
    Rcpp::stop("Vectors not matching");
  }
  
  for (arma::uword i = 0; i < m1.n_rows; ++i) {
    arma::ivec r1 = m1.row(i).t();
    total_distance += d(r1, r2);
  }
  return total_distance;
}

int CayleyDistance::d(const ivec& r1, const ivec& r2) {
  int distance{};
  ivec tmp2 = r1;
  
  for(size_t i{}; i < r1.n_elem; ++i){
    if(tmp2(i) != r2(i)) {
      distance += 1;
      int tmp1 = tmp2(i);
      tmp2(i) = r2(i);
      uvec inds = find(tmp2 == r2(i));
      tmp2.elem(inds).fill(tmp1);
    }
  }
  return distance;
}

int FootruleDistance::d(const ivec& r1, const ivec& r2) {
  int value{};
  for(size_t i{}; i < r1.size(); i++) {
    value += r1[i] > r2[i] ? r1[i] - r2[i] : r2[i] - r1[i];
  }
  return value;
}

int HammingDistance::d(const ivec& r1, const ivec& r2) {
  return sum(r1 != r2);
}

int KendallDistance::d(const ivec& r1, const ivec& r2) {
  int distance{};
  for(size_t i{}; i < r1.n_elem; ++i){
    for(size_t j{}; j < i; ++j){
      if (((r1(j) > r1(i)) && (r2(j) < r2(i)) ) ||
          ((r1(j) < r1(i)) && (r2(j) > r2(i)))) {
        distance += 1;
      }
    }
  }
  return distance;
}

int SpearmanDistance::d(const ivec& r1, const ivec& r2) {
  int value{};
  for(size_t i{}; i < r1.size(); i++) {
    value += r1[i] > r2[i] ? pow(r1[i] - r2[i], 2) : pow(r2[i] - r1[i], 2);
  }
  return value;
}

// Rewritten from https://www.geeksforgeeks.org/c-program-for-longest-increasing-subsequence/
int longest_increasing_subsequence(const ivec& permutation) {
  int n = permutation.n_elem;
  ivec lis(n, fill::ones);
  
  for (size_t i = 1; i < n; i++) {
    for (size_t j = 0; j < i; j++) {
      if (permutation(i) > permutation(j) && lis(i) < lis(j) + 1) {
        lis(i) = lis(j) + 1;
      }
    }
  }
  return max(lis);
}

int UlamDistance::d(const ivec& r1, const ivec& r2) {
  uvec x = sort_index(r2);
  return r1.size() - longest_increasing_subsequence(r1(x));
}
