#pragma once
#include "data.h"
#include "particle.h"

struct LatentRankingProposal{
  arma::umat proposal;
  double log_probability{};
  std::vector<std::string> new_users{};
  std::vector<std::string> updated_consistent_users{};
  std::vector<std::string> updated_inconsistent_users{};
};

LatentRankingProposal sample_latent_rankings(
  const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior,
  std::string latent_rank_proposal,
  const StaticParameters& parameters,
  const arma::umat& current_latent_rankings
);
LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t, const Prior& prior,
    std::string latent_rank_proposal,
    const StaticParameters& parameters,
    const arma::umat& current_latent_rankings);
LatentRankingProposal sample_latent_rankings(
    const PairwisePreferences* data, unsigned int t, const Prior& prior);

topological_sorts_user all_topological_sorts(const arma::umat& prefs, int n_items);
