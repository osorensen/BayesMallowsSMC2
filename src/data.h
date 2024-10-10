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
  virtual void update_observed_users(unsigned int t) = 0;
  std::vector<std::string> observed_users{};
  std::vector<std::string>::const_iterator find_user(const std::string& user_id) const;
  std::vector<std::string>::iterator find_user(const std::string& user_id);
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries);
  ranking_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_observed_users(unsigned int t) override;
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_observed_users(unsigned int t) override;
  std::string topological_sorts_directory;
  Rcpp::IntegerVector num_topological_sorts;
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);
