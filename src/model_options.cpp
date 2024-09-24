#include "model_options.h"

ModelOptions::ModelOptions(
  const Rcpp::List& input_model_options,
  const Rcpp::List& input_data
) : metric ( input_model_options["metric"] ),
  n_clusters{ input_model_options["n_clusters"] },
  n_items { input_data["n_items"]} {}