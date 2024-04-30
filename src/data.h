#pragma once
#include <RcppArmadillo.h>
#include <memory>
#include <algorithm>
#include "typedefs.h"
#include "prior.h"

struct Data{
  Data(){};
  virtual ~Data() = default;
  virtual void print() = 0;
  virtual unsigned int n_timepoints() = 0;
  virtual void update_topological_sorts(unsigned int t, int n_items) = 0;
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries);
  ranking_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_topological_sorts(unsigned int t, int n_items) override { return; };
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
  void print() override;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_topological_sorts(unsigned int t, int n_items) override;
  topological_sorts_tp current_topological_sorts;
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);
