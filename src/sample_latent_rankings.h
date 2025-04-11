#pragma once
#include "data.h"
#include "particle.h"

struct LatentRankingProposal{
  arma::umat proposal{};
  arma::mat cluster_probabilities{};
  arma::uvec cluster_assignment{};
  arma::vec log_probability{};
  std::map<unsigned int, std::string> users{};
};

LatentRankingProposal sample_latent_rankings(
  const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior,
  std::string latent_rank_proposal,
  const StaticParameters& parameters,
  const std::unique_ptr<PartitionFunction>& pfun,
  const std::unique_ptr<Distance>& distfun
);
LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t,
    std::string latent_rank_proposal,
    const StaticParameters& parameters,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun);
LatentRankingProposal sample_latent_rankings(
    const PairwisePreferences* data, unsigned int t, const Prior& prior);

