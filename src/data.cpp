#include <math.h>
#include "data.h"
#include "misc.h"
using namespace arma;

Rankings::Rankings(const Rcpp::List& input_timeseries) {
  timeseries.reserve(input_timeseries.size());
  for(Rcpp::List a : input_timeseries) {
    ranking_tp new_data;
    new_data.reserve(a.size());
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      uvec rankings = a[i];
      new_data.push_back(ranking_obs(nm[i], rankings));
    }
    timeseries.push_back(new_data);
  }
}

void Rankings::print() {
  for(auto timepoint : timeseries) {
    for(auto observation : timepoint) {
      Rcpp::Rcout << "user " << observation.first << std::endl
                  << "rankings " << observation.second.t() << std::endl;
    }
  }
}

PairwisePreferences::PairwisePreferences(const Rcpp::List& input_timeseries) {
  timeseries.reserve(input_timeseries.size());
  for(Rcpp::List a : input_timeseries) {
    pairwise_tp new_data;
    new_data.reserve(a.size());
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      umat preferences = a[i];
      comparisons user_data;
      for(size_t j{}; j < preferences.n_rows; j++) {
        user_data.insert(single_comparison(preferences(j, 0), preferences(j, 1)));
      }
      new_data.push_back(pairwise_obs(nm[i], user_data));
    }
    timeseries.push_back(new_data);
  }
}

void PairwisePreferences::print() {
  for(auto timepoint : timeseries) {
    for(auto observation : timepoint) {
      Rcpp::Rcout << "user " << observation.first << std::endl
                  << "preferences ";
      auto comparisons{observation.second};
      std::for_each(comparisons.begin(), comparisons.end(),
                    [](const single_comparison& s ){
                      Rcpp::Rcout << s.first << "<" << s.second << ", ";
                    });
      Rcpp::Rcout << std::endl;
    }
    Rcpp::Rcout << std::endl;
  }
}

LatentRankingProposal Rankings::sample_latent_rankings(
    unsigned int t, const Prior& prior) {
  LatentRankingProposal proposal;
  ranking_tp new_data = timeseries[t];
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

LatentRankingProposal PairwisePreferences::sample_latent_rankings(
    unsigned int t, const Prior& prior) {

  return LatentRankingProposal();
}

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries) {
  if(strcmp(input_timeseries.attr("type"), "complete rankings") == 0) {
    return std::make_unique<Rankings>(input_timeseries);
  } else if(strcmp(input_timeseries.attr("type"), "partial rankings") == 0) {
    return std::make_unique<Rankings>(input_timeseries);
  } else if(strcmp(input_timeseries.attr("type"), "pairwise preferences") == 0) {
    return std::make_unique<PairwisePreferences>(input_timeseries);
  } else {
    Rcpp::stop("Wrong data type.");
  }
}
