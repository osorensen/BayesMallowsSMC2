#include <RcppArmadillo.h>
#include "sample_latent_rankings.h"
#include "data.h"
#include "misc.h"
using namespace arma;

LatentRankingProposal sample_latent_rankings(
    const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior
) {
  if(Rankings* r = dynamic_cast<Rankings*>(data.get())) {
    return sample_latent_rankings(r, t, prior);
  } else if (PairwisePreferences* pp = dynamic_cast<PairwisePreferences*>(data.get())) {
    return sample_latent_rankings(pp, t, prior);
  } else {
    Rcpp::stop("Unknown type.");
  }
}

LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t, const Prior& prior) {
  LatentRankingProposal proposal;
  ranking_tp new_data = data->timeseries[t];
  uvec all_items = regspace<uvec>(0, prior.n_items - 1);
  uvec all_rankings = regspace<uvec>(1, prior.n_items);
  proposal.proposal = umat(prior.n_items, new_data.size());

  for(size_t i{}; i < new_data.size(); i++) {
    uvec observed_ranking = new_data[i].second;
    uvec observed_items = find(observed_ranking);
    uvec available_items = setdiff(all_items, observed_items);
    uvec available_rankings = setdiff(all_rankings, observed_ranking);
    uvec tmp = observed_ranking;
    tmp(available_items) = shuffle(available_rankings);
    proposal.proposal.col(i) = tmp;

    proposal.log_probability -= lgamma(available_rankings.size() + 1.0);
  }

  return proposal;
}

LatentRankingProposal sample_latent_rankings(
    const PairwisePreferences* data, unsigned int t, const Prior& prior) {
  LatentRankingProposal proposal;
  topological_sorts_tp new_data = data->current_topological_sorts;
  proposal.proposal = umat(prior.n_items, new_data.size());

  for(size_t i{}; i < new_data.size(); i++) {
    int sampled_index = randi(distr_param(0, new_data[i].size() - 1));
    proposal.proposal.col(i) = conv_to<uvec>::from(new_data[i][sampled_index]);
    proposal.log_probability = -log(new_data[i].size());
  }

  return proposal;
}
