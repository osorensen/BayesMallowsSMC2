// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include <numeric>
#include "prior.h"
#include "data.h"
#include "particle.h"
#include "options.h"
#include "partition_functions.h"
#include "distances.h"
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
  auto pfun = choose_partition_function(prior.n_items, options.metric);
  auto distfun = choose_distance_function(options.metric);

  int T = data->n_timepoints();
  for(size_t t{}; t < T; t++) {
    Rcpp::Rcout << "t = " << t << std::endl;
    for(auto& p : particle_vector) {
      p.run_particle_filter(t, prior, data, pfun, distfun);
      p.log_importance_weight += p.log_incremental_likelihood(t);
    }

    vec log_importance_weights(particle_vector.size());
    std::transform(
      particle_vector.cbegin(), particle_vector.cend(),
      log_importance_weights.begin(),
      [](const Particle& p) { return p.log_importance_weight; });

    vec log_normalized_importance_weights =
      log_importance_weights - (max(log_importance_weights +
      log(sum(exp(log_importance_weights - max(log_importance_weights))))));

    double ess = pow(norm(exp(log_normalized_importance_weights), 2), -2);

    Rcpp::Rcout << "ess = " << ess << std::endl;
    if(ess < options.resampling_threshold) {
      ivec new_inds = Rcpp::sample(
        log_normalized_importance_weights.size(),
        log_normalized_importance_weights.size(), true,
        Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(exp(log_normalized_importance_weights))), false);
      std::vector<Particle> tmp = particle_vector;
      for(size_t i{}; i < new_inds.size(); i++) {
        particle_vector[i] = tmp[new_inds[i]];
      }

      std::for_each(particle_vector.begin(), particle_vector.end(),
                    [](Particle& p) { p.log_importance_weight = 1; });
    }

  }

  return Rcpp::List::create(
    Rcpp::Named("a") = "b"
  );
}
