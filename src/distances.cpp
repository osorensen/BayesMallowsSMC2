#include "distances.h"
using namespace arma;

std::unique_ptr<Distance> choose_distance_function(const std::string& metric) {
  if(metric == "cayley") {
    return std::make_unique<CayleyDistance>();
  } else if(metric == "footrule") {
    return std::make_unique<FootruleDistance>();
  } else if(metric == "hamming") {
    return std::make_unique<HammingDistance>();
  } else if(metric == "kendall") {
    return std::make_unique<KendallDistance>();
  } else if(metric == "spearman") {
    return std::make_unique<SpearmanDistance>();
  } else if(metric == "ulam") {
    return std::make_unique<UlamDistance>();
  } else {
    Rcpp::stop("Unknown metric.");
  }
}

unsigned int CayleyDistance::d(const uvec& r1, const uvec& r2) {
  unsigned int distance{};
  uvec tmp2 = r1;

  for(size_t i{}; i < r1.n_elem; ++i){
    if(tmp2(i) != r2(i)) {
      distance += 1;
      unsigned int tmp1 = tmp2(i);
      uvec inds = find(tmp2 == r2(i));
      tmp2(i) = r2(i);
      tmp2.elem(inds).fill(tmp1);
    }
  }
  return distance;
}

unsigned int FootruleDistance::d(const uvec& r1, const uvec& r2) {
  unsigned int value{};
  for(size_t i{}; i < r1.size(); i++) {
    value += r1[i] > r2[i] ? r1[i] - r2[i] : r2[i] - r1[i];
  }
  return value;
}

unsigned int HammingDistance::d(const uvec& r1, const uvec& r2) {
  return sum(r1 != r2);
}

unsigned int KendallDistance::d(const uvec& r1, const uvec& r2) {
  unsigned int distance{};
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

unsigned int SpearmanDistance::d(const uvec& r1, const uvec& r2) {
  unsigned int value{};
  for(size_t i{}; i < r1.size(); i++) {
    value += r1[i] > r2[i] ? pow(r1[i] - r2[i], 2) : pow(r2[i] - r1[i], 2);
  }
  return value;
}

// Rewritten from https://www.geeksforgeeks.org/c-program-for-longest-increasing-subsequence/
unsigned int longest_increasing_subsequence(const uvec& permutation) {
  unsigned int n = permutation.n_elem;
  uvec lis(n, fill::ones);

  for (size_t i = 1; i < n; i++) {
    for (size_t j = 0; j < i; j++) {
      if (permutation(i) > permutation(j) && lis(i) < lis(j) + 1) {
        lis(i) = lis(j) + 1;
      }
    }
  }
  return max(lis);
}

unsigned int UlamDistance::d(const uvec& r1, const uvec& r2) {
  uvec x = sort_index(r2);
  return r1.size() - longest_increasing_subsequence(r1(x));
}
