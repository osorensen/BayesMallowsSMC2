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
  virtual void update_observed_users(unsigned int t) = 0;
  virtual void prune(unsigned int t) = 0;
  virtual void unprune() = 0;
  std::map<std::string, unsigned int> observed_users{}; // user ID and index in latent ranking and cluster
  bool updated_users{};
};

struct Rankings : Data {
  Rankings(const Rcpp::List& input_timeseries, bool partial_rankings);
  ranking_ts timeseries;
  ranking_ts original_timeseries;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_observed_users(unsigned int t) override;
  bool partial_rankings{};
  void prune(unsigned int t) override;
  void unprune() override;
};

struct PairwisePreferences : Data{
  PairwisePreferences(const Rcpp::List& input_timeseries);
  pairwise_ts timeseries;
  pairwise_ts original_timeseries;
  unsigned int n_timepoints() override { return timeseries.size(); }
  void update_observed_users(unsigned int t) override;
  std::string topological_sorts_directory;
  Rcpp::IntegerVector num_topological_sorts;
  void prune(unsigned int t) override;
  void unprune() override;
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries);

template<typename T>
T prune_worker(const T& timeseries, unsigned int t) {
  if(t >= timeseries.size()) return;
  auto& target_tp = timeseries[t];

  std::set<std::string> keys_to_prune;
  for(const auto& [key, _] : target_tp) {
    keys_to_prune.insert(key);
  }

  T new_timeseries;
  for(size_t s{}; s < t; s++) {
    bool should_prune{};
    for(const auto& [key, _] : timeseries[s]) {
      if(keys_to_prune.find(key) != keys_to_prune.end()) {
        should_prune = true;
        break;
      }
    }
    if(!should_prune) {
      new_timeseries.push_back(std::move(timeseries[s]));
    }
  }

  for(size_t s{}; s < timeseries.size(); s++) {
    new_timeseries.push_back(std::move(timeseries[s]));
  }

  timeseries = std::move(new_timeseries);
}
