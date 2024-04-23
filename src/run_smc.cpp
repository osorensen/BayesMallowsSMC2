// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
using namespace arma;

// [[Rcpp::export]]
Rcpp::List run_smc(
  Rcpp::List data
) {

  return Rcpp::List::create(
    Rcpp::Named("a") = "b"
  );
}
