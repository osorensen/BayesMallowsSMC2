#pragma once
#include <RcppArmadillo.h>
arma::uvec setdiff(const arma::uvec& x1, const arma::uvec& x2);
double log_sum_exp(const arma::vec& x);
double log_mean_exp(const arma::vec& x);
arma::vec softmax(const arma::vec& x);
