// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include <string>
#include "prior.h"
#include "data.h"
using namespace arma;

// [[Rcpp::export]]
Rcpp::List run_smc(
  Rcpp::List input_timeseries,
  Rcpp::List input_prior
) {

  Prior prior{input_prior};
  auto data = setup_data(input_timeseries);
  //data->print();

  return Rcpp::List::create(
    Rcpp::Named("a") = "b"
  );
}
