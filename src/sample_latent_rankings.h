#pragma once
#include "data.h"

struct LatentRankingProposal{
  arma::umat proposal;
  double log_probability{};
};

LatentRankingProposal sample_latent_rankings(
  const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior
);
LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t, const Prior& prior);
LatentRankingProposal sample_latent_rankings(
    const PairwisePreferences* data, unsigned int t, const Prior& prior);

topological_sorts_user all_topological_sorts(const arma::umat& prefs, int n_items);
