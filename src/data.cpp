#include "data.h"
#include <string>

using namespace arma;

MatrixData::MatrixData(const Rcpp::List& input_data, const std::string& key)
  : matrix(input_data[key]) {
  matrix.col(0) -= 1;
}

int MatrixData::n_timepoints() {
  ivec timepoints = unique(matrix.col(0));
  return timepoints.n_elem;
}

imat MatrixData::get_timepoint_data(int t) {
  uvec indices = find(matrix.col(0) == t);
  imat submatrix = matrix.rows(indices);
  return submatrix.cols(2, submatrix.n_cols - 1);
}

CompleteRankings::CompleteRankings(const Rcpp::List& input_data)
  : MatrixData(input_data, "x") {}

PartialRankings::PartialRankings(const Rcpp::List& input_data)
  : MatrixData(input_data, "x") {}

Preferences::Preferences(const Rcpp::List& input_data)
  : MatrixData(input_data, "x") {}

std::unique_ptr<Data> setup_data(const Rcpp::List& input_data) {
  std::string type = Rcpp::as<std::string>(input_data["type"]);
  
  if (type == "complete_rankings") {
    return std::make_unique<CompleteRankings>(input_data);
  } else if (type == "partial_rankings") {
    return std::make_unique<PartialRankings>(input_data);
  } else if (type == "preferences") {
    return std::make_unique<Preferences>(input_data);
  } else {
    Rcpp::stop("Unknown data type.");
  }
}
