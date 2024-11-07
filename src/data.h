#pragma once
#include <RcppArmadillo.h>
#include <memory>
#include <algorithm>
#include <vector>
#include <string>
#include <map>
#include "typedefs.h"
#include "prior.h"

struct Data{
  Data(){};
  virtual ~Data() = default;
  virtual unsigned int n_timepoints() = 0;
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries, bool partial_rankings);
  ranking_ts timeseries;
  ranking_ts original_timeseries;
  unsigned int n_timepoints() override { return timeseries.size(); }
  bool partial_rankings{};
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
  pairwise_ts original_timeseries;
  unsigned int n_timepoints() override { return timeseries.size(); }
  std::string topological_sorts_directory;
  Rcpp::IntegerVector num_topological_sorts;
  Rcpp::IntegerVector file_count;
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);


