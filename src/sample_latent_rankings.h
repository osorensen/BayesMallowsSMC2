#pragma once
#include "data.h"
#include "particle.h"

struct LatentRankingProposal{
  arma::umat proposal{};
  arma::uvec cluster_assignment{};
  double log_probability{};
  // string shows index in data, integer shows index in proposal matrix
  std::map<std::string, unsigned int> updated_inconsistent_users{};
};

LatentRankingProposal sample_latent_rankings(
  const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior,
  std::string latent_rank_proposal,
  const StaticParameters& parameters,
  const arma::umat& current_latent_rankings,
  const std::unique_ptr<PartitionFunction>& pfun,
  const std::unique_ptr<Distance>& distfun
);
LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t,
    std::string latent_rank_proposal,
    const StaticParameters& parameters,
    const arma::umat& current_latent_rankings,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun);
LatentRankingProposal sample_latent_rankings(
    const PairwisePreferences* data, unsigned int t, const Prior& prior);

topological_sorts_user all_topological_sorts(const arma::umat& prefs, int n_items);
