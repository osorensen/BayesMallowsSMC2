#include <math.h>
#include "data.h"
#include "misc.h"
#include "sample_latent_rankings.h"
#include "update_users_helper.h"
using namespace arma;

Rankings::Rankings(const Rcpp::List& input_timeseries) {
  timeseries.reserve(input_timeseries.size());
  for(Rcpp::List a : input_timeseries) {
    ranking_tp new_data;
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      new_data[std::string(nm[i])] = uvec(a[i]);
    }
    timeseries.push_back(new_data);
  }
}

void Rankings::update_observed_users(unsigned int t) {
  updateObservedUsersHelper(observed_users, timeseries, t);
}

void PairwisePreferences::update_observed_users(unsigned int t) {
  updateObservedUsersHelper(observed_users, timeseries, t);
}

PairwisePreferences::PairwisePreferences(const Rcpp::List& input_timeseries) :
  topological_sorts_directory (input_timeseries.attr("topological_sorts_directory")),
  num_topological_sorts (input_timeseries.attr("num_topological_sorts")) {
  timeseries.reserve(input_timeseries.size());
  for(Rcpp::List a : input_timeseries) {
    pairwise_tp new_data;
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      umat preferences = a[i];
      comparisons user_data;
      for(size_t j{}; j < preferences.n_rows; j++) {
        user_data.insert(single_comparison(preferences(j, 0), preferences(j, 1)));
      }
      new_data[std::string(nm[i])] = user_data;
    }
    timeseries.push_back(new_data);
  }
}



std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries) {
  std::string type = Rcpp::as<std::string>(input_timeseries.attr("type"));

  if (type == "complete rankings" || type == "partial rankings") {
    return std::make_unique<Rankings>(input_timeseries);
  } else if (type == "pairwise preferences") {
    return std::make_unique<PairwisePreferences>(input_timeseries);
  } else {
    Rcpp::stop("Wrong data type.");
  }
}
