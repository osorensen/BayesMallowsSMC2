#pragma once
#include <RcppArmdillo.h>
#include "typedefs.h"

arma::uvec sample_latent_rankings(const ranking_obs& observation);
arma::uvec sample_latent_rankings(const pairwise_obs& observation);
