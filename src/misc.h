#pragma once
#include <RcppArmadillo.h>
arma::uvec setdiff(const arma::uvec& x1, const arma::uvec& x2);
arma::vec log_sum_exp(const arma::vec& x);
