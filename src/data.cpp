#include "data.h"
using namespace arma;

Rankings::Rankings(const Rcpp::List& input_timeseries) {
  timeseries.reserve(input_timeseries.size());
  for(Rcpp::List a : input_timeseries) {
    ranking_tp new_data;
    new_data.reserve(a.size());
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      uvec rankings = a[i];
      new_data.push_back(ranking_obs(nm[i], rankings));
    }
    timeseries.push_back(new_data);
  }
}

PairwisePreferences::PairwisePreferences(const Rcpp::List& input_timeseries) {
  timeseries.reserve(input_timeseries.size());
  for(Rcpp::List a : input_timeseries) {
    pairwise_tp new_data;
    new_data.reserve(a.size());
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      umat preferences = a[i];
      comparisons user_data;
      for(size_t j{}; j < preferences.n_rows; j++) {
        user_data.insert(single_comparison(preferences(j, 0), preferences(j, 1)));
      }
      new_data.push_back(pairwise_obs(nm[i], user_data));
    }
    timeseries.push_back(new_data);
  }
}

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries) {
  if(strcmp(input_timeseries.attr("type"), "complete rankings") == 0 ||
     strcmp(input_timeseries.attr("type"), "partial rankings") == 0) {
    return std::make_unique<Rankings>(input_timeseries);
  } else if(strcmp(input_timeseries.attr("type"), "pairwise preferences") == 0) {
    return std::make_unique<PairwisePreferences>(input_timeseries);
  } else {
    Rcpp::stop("Wrong data type.");
  }
}
