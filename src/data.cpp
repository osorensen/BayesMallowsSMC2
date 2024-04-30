#include <math.h>
#include "data.h"
#include "misc.h"
#include "sample_latent_rankings.h"
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

void Rankings::print() {
  for(auto timepoint : timeseries) {
    for(auto observation : timepoint) {
      Rcpp::Rcout << "user " << observation.first << std::endl
                  << "rankings " << observation.second.t() << std::endl;
    }
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

void PairwisePreferences::print() {
  for(auto timepoint : timeseries) {
    for(auto observation : timepoint) {
      Rcpp::Rcout << "user " << observation.first << std::endl
                  << "preferences ";
      auto comparisons{observation.second};
      std::for_each(comparisons.begin(), comparisons.end(),
                    [](const single_comparison& s ){
                      Rcpp::Rcout << s.first << "<" << s.second << ", ";
                    });
      Rcpp::Rcout << std::endl;
    }
    Rcpp::Rcout << std::endl;
  }
}

void PairwisePreferences::update_topological_sorts(unsigned int t, int n_items) {
  pairwise_tp new_data = timeseries[t];
  current_topological_sorts = topological_sorts_tp(new_data.size());
  for(size_t i{}; i < new_data.size(); i++) {
    comparisons user_preferences = new_data[i].second;
    umat preference_matrix{};
    for(auto it = user_preferences.begin(); it != user_preferences.end(); it++) {
      auto element = *it;
      preference_matrix = join_cols(preference_matrix, urowvec{element.first, element.second});
    }
    current_topological_sorts[i] = all_topological_sorts(preference_matrix, n_items);
  }
}

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries) {
  if(strcmp(input_timeseries.attr("type"), "complete rankings") == 0) {
    return std::make_unique<Rankings>(input_timeseries);
  } else if(strcmp(input_timeseries.attr("type"), "partial rankings") == 0) {
    return std::make_unique<Rankings>(input_timeseries);
  } else if(strcmp(input_timeseries.attr("type"), "pairwise preferences") == 0) {
    return std::make_unique<PairwisePreferences>(input_timeseries);
  } else {
    Rcpp::stop("Wrong data type.");
  }
}
