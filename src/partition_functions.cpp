#include "partition_functions.h"
#include <string>
#include <RcppArmadillo.h>
using namespace arma;

std::unique_ptr<PartitionFunction> choose_partition_function(
    int n_items, std::string metric) {
  if(metric == "cayley") {
    return std::make_unique<Cayley>(n_items);
  } else if(metric == "hamming") {
    return std::make_unique<Hamming>(n_items);
  } else if(metric == "kendall") {
    return std::make_unique<Kendall>(n_items);
  } else if(metric == "footrule" || metric == "spearman" || metric == "ulam") {
    return std::make_unique<Cardinalities>(n_items, metric);
  } else {
    Rcpp::stop("Unknown metric.");
  }
}

Cayley::Cayley(unsigned int n_items) : n_items { n_items } {}

double Cayley::logz(double alpha) {
  double res{};
  for(int i{1}; i < n_items; ++i){
    res += std::log(1.0 + i * std::exp(- alpha));
  }
  return res;
}

Hamming::Hamming(unsigned int n_items) : n_items { n_items } {}

double Hamming::logz(double alpha) {
  double res{};
  for(int i{}; i < (n_items + 1); ++i){
    res += tgamma(n_items + 1.0) * std::exp(-alpha) *
      std::pow((std::exp(alpha) - 1.0), i) / tgamma(i + 1.0);
  }
  return std::log(res);
}

Kendall::Kendall(unsigned int n_items) : n_items { n_items } {}

double Kendall::logz(double alpha) {
  double res{};
  for(int i{1}; i < (n_items + 1); ++i){
    res += std::log((1.0 - std::exp(-i * alpha)) / (1.0 - std::exp(-alpha)));
  }
  return res;
}

Cardinalities::Cardinalities(unsigned int n_items, const std::string& metric) :
  n_items { n_items } {
  mat tmp;
    Rcpp::Function f("system.file");
    Rcpp::List pkg_path = f(Rcpp::Named("package") = "BayesMallowsSMC2");

  std::string filename = std::string(pkg_path[0]) + std::string("/partition_function_data/") +
    metric + std::string("_") + std::to_string(n_items) + std::string("items.csv");

  bool ok = tmp.load(filename);
  if(ok == false) {
    Rcpp::stop("Could not find partition function.");
  }

  distances = tmp.col(0);
  cardinalities = tmp.col(1);
}

double Cardinalities::logz(double alpha) {
  return std::log(accu(cardinalities % exp(-distances * alpha)));
}


