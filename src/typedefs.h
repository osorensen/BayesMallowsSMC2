#pragma once
#include <map>
#include <RcppArmadillo.h>
#include <string>
#include <utility>

struct RankingObs {
  arma::uvec observation{};
  arma::uvec available_items{};
  arma::uvec available_rankings{};
};

using ranking_tp = std::map<std::string, RankingObs>;
using ranking_ts = std::vector<ranking_tp>;
using single_comparison = std::pair<unsigned int, unsigned int>;
using comparisons = std::set<single_comparison>;
using pairwise_tp = std::map<std::string, comparisons>;
using pairwise_ts = std::vector<pairwise_tp>;
using sort_matrices_tp = std::map<std::string, arma::umat>;
using sort_matrices_ts = std::vector<sort_matrices_tp>;
using sort_counts_tp = std::map<std::string, long long int>;
using sort_counts_ts = std::vector<sort_counts_tp>;
