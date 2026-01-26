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
      // 1. Read as Rcpp vector to safely check for NA
      Rcpp::NumericVector r_vec = a[i];

      // 2. Create a clean Armadillo vector, forcing NAs to 0
      arma::uvec clean_vec(r_vec.size());
      for(int j = 0; j < r_vec.size(); ++j) {
        if (Rcpp::NumericVector::is_na(r_vec[j])) {
          clean_vec[j] = 0; // Explicitly mark as missing
        } else {
          clean_vec[j] = (unsigned int)r_vec[j];
        }
      }

      // 3. Use the clean vector
      new_data[std::string(nm[i])] = RankingObs{
        clean_vec,
        find_available_items(clean_vec),
        find_available_rankings(clean_vec)
      };
    }
    timeseries.push_back(new_data);
  }
  original_timeseries = timeseries;
}

PairwisePreferences::PairwisePreferences(
  const Rcpp::List& input_timeseries,
  const Rcpp::List& input_sort_matrices,
  const Rcpp::List& input_sort_counts
) {
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

  sort_matrix_timeseries.reserve(input_sort_matrices.size());

  for(Rcpp::List a : input_sort_matrices) {
    sort_matrices_tp new_data;
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      umat sort_matrix = a[i];
      new_data[std::string(nm[i])] = sort_matrix;
    }
    sort_matrix_timeseries.push_back(new_data);
  }

  sort_count_timeseries.reserve(input_sort_counts.size());
  for(Rcpp::List a : input_sort_counts) {
    sort_counts_tp new_data;
    Rcpp::CharacterVector nm = a.names();
    for(size_t i{}; i < nm.size(); i++) {
      new_data[std::string(nm[i])] = a[i];
    }
    sort_count_timeseries.push_back(new_data);
  }
}

std::unique_ptr<Data> setup_data(
    const Rcpp::List& input_timeseries,
    const Rcpp::List& input_sort_matrices,
    const Rcpp::List& input_sort_counts
) {
  std::string type = Rcpp::as<std::string>(input_timeseries.attr("type"));

  if (type == "complete rankings" || type == "partial rankings") {
    return std::make_unique<Rankings>(input_timeseries, type == "partial rankings");
  } else if (type == "pairwise preferences") {
    return std::make_unique<PairwisePreferences>(
      input_timeseries, input_sort_matrices, input_sort_counts
      );
  } else {
    Rcpp::stop("Wrong data type.");
  }
}

