#include <RcppArmadillo.h>
#include <algorithm>
#include <vector>

using namespace arma;

uvec setdiff(const uvec& x1, const uvec& x2) {
  std::vector<unsigned int> x1_vec(x1.begin(), x1.end());
  std::vector<unsigned int> x2_vec(x2.begin(), x2.end());
  std::sort(x1_vec.begin(), x1_vec.end());
  std::sort(x2_vec.begin(), x2_vec.end());

  std::vector<unsigned int> difference;
  std::set_difference(
    x1_vec.begin(), x1_vec.end(), x2_vec.begin(), x2_vec.end(),
    std::back_inserter(difference)
  );

  return arma::uvec(difference);
}

vec log_sum_exp(const vec& x) {
  double maxval = x.max();
  return x - (maxval + log(sum(exp(x - maxval))));
}
