#include <math.h>
#include "data.h"
#include "misc.h"
#include "sample_latent_rankings.h"

using namespace arma;

uvec find_available_rankings(const uvec& observed_ranking) {
  uvec all_rankings = regspace<uvec>(1, observed_ranking.size());
  return setdiff(all_rankings, observed_ranking);
}

uvec find_available_items(const uvec& observed_ranking) {
  uvec available_items = regspace<uvec>(0, observed_ranking.size() - 1);
  return setdiff(available_items, find(observed_ranking));
}

Rankings::Rankings(const Rcpp::List& input_timeseries, bool partial_rankings) :
  partial_rankings { partial_rankings } {
  timeseries.reserve(input_timeseries.size());
  for(Rcpp::List a : input_timeseries) {
    ranking_tp new_data;
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      new_data[std::string(nm[i])] = RankingObs{uvec(a[i]), find_available_items(uvec(a[i])), find_available_rankings(uvec(a[i]))};
    }
    timeseries.push_back(new_data);
  }
  original_timeseries = timeseries;
}

PairwisePreferences::PairwisePreferences(const Rcpp::List& input_timeseries) :
  topological_sorts_directory (input_timeseries.attr("topological_sorts_directory")),
  num_topological_sorts (input_timeseries.attr("num_topological_sorts")),
  file_count (input_timeseries.attr("file_count")) {
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
  original_timeseries = timeseries;
}

std::unique_ptr<Data> setup_data(const Rcpp::List& input_timeseries) {
  std::string type = Rcpp::as<std::string>(input_timeseries.attr("type"));

  if (type == "complete rankings" || type == "partial rankings") {
    return std::make_unique<Rankings>(input_timeseries, type == "partial rankings");
  } else if (type == "pairwise preferences") {
    return std::make_unique<PairwisePreferences>(input_timeseries);
  } else {
    Rcpp::stop("Wrong data type.");
  }
}

