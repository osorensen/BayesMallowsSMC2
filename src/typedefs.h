#pragma once
#include <string>
#include <utility>
#include <set>

using ranking_obs = std::pair<std::string, arma::uvec>;
using ranking_tp = std::vector<ranking_obs>;
using ranking_ts = std::vector<ranking_tp>;
using single_comparison = std::pair<unsigned int, unsigned int>;
using comparisons = std::set<single_comparison>;
using pairwise_obs = std::pair<std::string, comparisons>;
using pairwise_tp = std::vector<pairwise_obs>;
using pairwise_ts= std::vector<pairwise_tp>;
