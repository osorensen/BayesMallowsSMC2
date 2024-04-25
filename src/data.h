#pragma once
#include <RcppArmadillo.h>
#include <memory>
#include <algorithm>
#include "typedefs.h"

struct Data{
  Data(){};
  virtual ~Data() = default;
  virtual void print() = 0;
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries);
  ranking_ts timeseries;
  void print();
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
  void print();
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);
