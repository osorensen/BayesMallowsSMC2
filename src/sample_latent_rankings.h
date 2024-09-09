#pragma once
#include "data.h"
#include "particle.h"

struct LatentRankingProposal{
  arma::umat proposal;
  double log_probability{};
  std::unordered_set<std::string> new_users{};
  std::unordered_set<std::string> updated_consistent_users{};
};

LatentRankingProposal sample_latent_rankings(
  const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior,
  std::string latent_rank_proposal,
  const StaticParameters& parameters
);
LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t, const Prior& prior,
    std::string latent_rank_proposal,
    const StaticParameters& parameters);
LatentRankingProposal sample_latent_rankings(
    const PairwisePreferences* data, unsigned int t, const Prior& prior);

topological_sorts_user all_topological_sorts(const arma::umat& prefs, int n_items);
