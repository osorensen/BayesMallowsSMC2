#pragma once
#include <RcppArmadillo.h>

#include "data.h"

struct SMCOptions{
  SMCOptions(
    const std::unique_ptr<Data>& data,
    const Rcpp::List& input_smc_options
  );
  ~SMCOptions() = default;
  
  int n_particles{};
  int n_particle_filters{};
  std::string resampler{};
  int max_rejuvenation_steps{};
  std::string latent_rank_proposer{"uniform"};
};