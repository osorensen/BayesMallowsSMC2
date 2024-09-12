#pragma once

#include <RcppArmadillo.h>

arma::vec softmax(const arma::vec& v);

template <typename T>
arma::vec extract_weights(const std::vector<T>& container, int t) {
  arma::vec log_weights(container.size());
  std::transform(
    container.cbegin(), container.cend(),
    log_weights.begin(),
    [t](const T& element) { return element.log_weight(t); }
  );
  
  return log_weights;
}