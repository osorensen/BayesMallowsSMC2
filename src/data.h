#pragma once
#include <RcppArmadillo.h>
#include <memory>
#include <algorithm>
#include "typedefs.h"
#include "prior.h"

struct LatentRankingProposal{
  arma::umat proposal;
  double log_probability{};
};

struct Data{
  Data(){};
  virtual ~Data() = default;
  virtual void print() = 0;
  virtual unsigned int n_timepoints() = 0;
  virtual LatentRankingProposal sample_latent_rankings(unsigned int t, const Prior& prior) = 0;
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries);
  ranking_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  LatentRankingProposal sample_latent_rankings(unsigned int t, const Prior& prior) override;
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  LatentRankingProposal sample_latent_rankings(unsigned int t, const Prior& prior) override;
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);
