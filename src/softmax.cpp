#include "softmax.h"

using namespace arma;

vec softmax(const vec& v) {
  double max_val = v.max();
  vec exp_shifted = exp(v - max_val);
  return exp_shifted / sum(exp_shifted);
}
