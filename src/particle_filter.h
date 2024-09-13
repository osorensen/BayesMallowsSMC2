#pragma once
#include <RcppArmadillo.h>

#include "data.h"
#include "prior.h"
#include "smc_options.h"

struct ParticleFilter{
  ParticleFilter(
    const std::unique_ptr<Data>& data,
    const Prior& prior
  );
  arma::vec log_weight{};
  arma::ivec cluster_labels{};
  arma::imat latent_rankings{};
  arma::ivec extract_latent_ranking(int u);
  void insert_latent_ranking(const arma::ivec& r, int u);
};

std::vector<ParticleFilter> create_particle_filter_vectors(
  const SMCOptions& smc_options,
  const std::unique_ptr<Data>& data,
  const Prior& prior
);

bool check_rank_consistency(
    const arma::ivec& latent, 
    const arma::ivec& observed,
    const arma::uvec& observed_inds
);