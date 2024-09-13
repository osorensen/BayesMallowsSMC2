#include <RcppArmadillo.h>
// [[Rcpp::depends(RcppArmadillo)]]

#include "data.h"
#include "distances.h"
#include "model_options.h"
#include "particle.h"
#include "partition_functions.h"
#include "post_process.h"
#include "prior.h"
#include "resampler.h"
#include "smc_options.h"
#include "softmax.h"

using namespace arma;

// [[Rcpp::export]]
Rcpp::List run_smc(
  Rcpp::List input_data,
  Rcpp::List input_smc_options,
  Rcpp::List input_prior,
  Rcpp::List input_model_options
) {
  Prior prior{input_prior, input_data};
  auto data = setup_data(input_data);
  SMCOptions smc_options{data, input_smc_options};
  ModelOptions model_options{input_model_options};
  auto distfun = choose_distance_function(model_options);
  auto resampler = choose_resampler(smc_options);
  auto pfun = choose_partition_function(prior, model_options);
  int T = data->n_timepoints();
  auto particle_vector = create_particle_vector(prior, smc_options, data);
  vec log_marginal_likelihood(T);

  for(size_t t{}; t < T; t++) {
    for(auto& particle : particle_vector) {
      particle.run_particle_filter(t, data, distfun, pfun);
    }

    vec log_weights = extract_weights(particle_vector, t);
    vec normalized_weights = softmax(log_weights);
    log_marginal_likelihood(t) = extract_marginal_likelihood(
      particle_vector, normalized_weights, t
    );

    double ess = 1.0 / pow(norm(normalized_weights), 2.0);
    
    if(ess < smc_options.n_particles / 2) {
      ivec new_inds =
        resampler->resample(smc_options.n_particles, normalized_weights);
      particle_vector = replace_elements(particle_vector, new_inds);

      AlphaSummaries alpha_summaries =
        compute_alpha_summaries(particle_vector, prior);
      
      int unq{};
      int iter{};
      do{
        for(auto& particle : particle_vector) {
          particle.rejuvenate(t, data, distfun, pfun, prior,
                              smc_options, alpha_summaries);
        }
        mat alpha_values = extract_alpha_values(particle_vector, prior);
        unq = count_unique_cols(alpha_values);
        iter++;
      } while (unq < particle_vector.size() / 2.0 && 
        iter < smc_options.max_rejuvenation_steps);
    }
  }

  return Rcpp::List::create(
    Rcpp::Named("alpha") = extract_alpha_values(particle_vector, prior),
    Rcpp::Named("rho") = extract_rho_values(particle_vector, prior),
    Rcpp::Named("tau") = extract_tau_values(particle_vector, prior),
    Rcpp::Named("weights") = softmax(extract_weights(particle_vector, T - 1)),
    Rcpp::Named("log_marginal_likelihood") = log_marginal_likelihood
  );
}

