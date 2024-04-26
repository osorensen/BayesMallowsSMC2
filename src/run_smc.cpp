// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include <string>
#include "prior.h"
#include "data.h"
#include "particle.h"
#include "options.h"
using namespace arma;

// [[Rcpp::export]]
Rcpp::List run_smc(
  Rcpp::List input_timeseries,
  Rcpp::List input_prior,
  Rcpp::List input_options
) {

  Prior prior{input_prior};
  Options options{input_options};
  auto data = setup_data(input_timeseries);
  auto particle_vector = create_particle_vector(options, prior);

  for(size_t t{}; t < 2; t++) {
    for(auto& p : particle_vector) {
      p.run_particle_filter(t);
    }
  }

  return Rcpp::List::create(
    Rcpp::Named("a") = "b"
  );
}
