#include "model_options.h"

ModelOptions::ModelOptions(
  const Rcpp::List& input_model_options
) : metric ( input_model_options["metric"] ) {}