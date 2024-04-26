#include "prior.h"

Prior::Prior(const Rcpp::List& input_prior) :
  alpha_shape { input_prior["alpha_shape"] },
  alpha_rate { input_prior["alpha_rate"] },
  cluster_concentration { input_prior["cluster_concentration"] },
  n_clusters { input_prior["n_clusters"] },
  n_items { input_prior["n_items"]} {}
