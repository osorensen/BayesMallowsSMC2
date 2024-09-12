#pragma once
#include <RcppArmadillo.h>
#include "prior.h"

arma::ivec leap_and_shift(const arma::ivec& current_rho, const Prior& prior);