#pragma once
#include <algorithm>
#include <map>
#include <RcppArmadillo.h>
#include "typedefs.h"

template<typename TimeSeriesType>
void updateObservedUsersHelper(
    std::map<std::string, unsigned int>& observed_users, const TimeSeriesType& timeseries, unsigned int t) {
  int current_max = 0;
  if (!observed_users.empty()) {
    auto max_iter = std::max_element(
      observed_users.begin(), observed_users.end(),
      [](const std::pair<std::string, int>& a, const std::pair<std::string, int>& b) {
        return a.second < b.second;
      }
    );
    current_max = max_iter->second;
  }
  int new_value = current_max + 1;
  for (auto it = timeseries[t].begin(); it != timeseries[t].end(); ++it) {
    if (observed_users.find(it->first) == observed_users.end()) {
      observed_users[it->first] = new_value++;
    } else {
      Rcpp::Rcout << "User " << it->first << " already exists." << std::endl;
    }
  }
}
