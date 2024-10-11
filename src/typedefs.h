#pragma once
#include <map>
#include <RcppArmadillo.h>
#include <string>
#include <utility>

using ranking_tp = std::map<std::string, arma::uvec>;
using ranking_ts = std::vector<ranking_tp>;
using single_comparison = std::pair<unsigned int, unsigned int>;
using comparisons = std::set<single_comparison>;
using pairwise_tp = std::map<std::string, comparisons>;
using pairwise_ts = std::vector<pairwise_tp>;

using topological_sorts_user = std::vector<arma::ivec>;
using topological_sorts_tp = std::vector<topological_sorts_user>;
