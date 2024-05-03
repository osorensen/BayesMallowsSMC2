#pragma once
#include <RcppArmadillo.h>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <set>
#include "typedefs.h"
#include "prior.h"

struct Data{
  Data(){};
  virtual ~Data() = default;
  virtual void print() = 0;
  virtual unsigned int n_timepoints() = 0;
  virtual void update_topological_sorts(unsigned int t, int n_items) = 0;
  virtual void update_observed_users(unsigned int t) = 0;
  std::set<std::string> observed_users{};
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries);
  ranking_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_topological_sorts(unsigned int t, int n_items) override { return; };
  void update_observed_users(unsigned int t) override;
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_topological_sorts(unsigned int t, int n_items) override;
  void update_observed_users(unsigned int t) override;
  topological_sorts_tp current_topological_sorts;
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);
