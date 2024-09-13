#pragma once
#include <RcppArmadillo.h>

#include "prior.h"
#include "smc_options.h"

struct LatentRankProposal{
  arma::ivec proposal;
  double log_probability;
};

struct LatentProposer{
  LatentProposer() {};
  virtual ~LatentProposer() = default;
  virtual LatentRankProposal propose(
      const arma::ivec& ranking,
      const Prior& prior) = 0;
};

struct Uniform : LatentProposer {
  LatentRankProposal propose(
      const arma::ivec& ranking, 
      const Prior& prior) override;
};

std::unique_ptr<LatentProposer> choose_latent_proposer(const SMCOptions& smc_options);