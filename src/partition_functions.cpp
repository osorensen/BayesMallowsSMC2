#include "partition_functions.h"

std::unique_ptr<PartitionFunction> choose_partition_function(
    int n_items, std::string metric) {
  if(metric == "cayley") {
    return std::make_unique<Cayley>(n_items);
  } else if(metric == "hamming") {
    return std::make_unique<Hamming>(n_items);
  } else if(metric == "kendall") {
    return std::make_unique<Kendall>(n_items);
  } else if(metric == "footrule") {
    return std::make_unique<Footrule>(n_items);
  } else if(metric == "spearman") {
    return std::make_unique<Spearman>(n_items);
  } else if(metric == "ulam") {
    return std::make_unique<Ulam>(n_items);
  } else {
    Rcpp::stop("Unknown metric.");
  }
}

Cayley::Cayley(unsigned int n_items) : n_items { n_items } {}

double Cayley::logz(double alpha) {
  double res{};
  for(int i{1}; i < n_items; ++i){
    res += std::log(1.0 + i * std::exp(- alpha / n_items));
  }
  return res;
}

Hamming::Hamming(unsigned int n_items) : n_items { n_items } {}

double Hamming::logz(double alpha) {
  double res{};
  for(int i{}; i < (n_items + 1); ++i){
    res += tgamma(n_items + 1.0) * std::exp(-alpha) *
      std::pow((std::exp(alpha / n_items) - 1.0), i) / tgamma(i + 1.0);
  }
  return std::log(res);
}

Kendall::Kendall(unsigned int n_items) : n_items { n_items } {}

double Kendall::logz(double alpha) {
  double res{};
  for(int i{1}; i < (n_items + 1); ++i){
    res += std::log((1.0 - std::exp(-i * alpha / n_items)) /
      (1.0 - std::exp(-alpha / n_items)));
  }
  return res;
}

Footrule::Footrule(unsigned int n_items) : n_items { n_items } {

}

double Footrule::logz(double alpha) {
  return std::log(arma::sum(cardinalities % exp(-alpha / n_items * distances)));
}
