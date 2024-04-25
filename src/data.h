#pragma once
#include <RcppArmadillo.h>
#include <memory>
#include "typedefs.h"

struct Data{
  Data(){};
  ~Data() = default;
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries);
  ranking_ts timeseries;
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);
