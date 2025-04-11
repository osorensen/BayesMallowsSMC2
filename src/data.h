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
  PairwisePreferences(
    const Rcpp::List& input_timeseries,
    const Rcpp::List& input_sort_matrices,
    const Rcpp::List& input_sort_counts
  );
  pairwise_ts timeseries;
  pairwise_ts original_timeseries;
  unsigned int n_timepoints() override { return timeseries.size(); }
  sort_matrices_ts sort_matrix_timeseries;
  sort_counts_ts sort_count_timeseries;
};

std::unique_ptr<Data> setup_data(
    const Rcpp::List& input_timeseries,
    const Rcpp::List& input_sort_matrices,
    const Rcpp::List& input_sort_counts
);


