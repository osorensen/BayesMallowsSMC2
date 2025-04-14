// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include "prior.h"
#include "data.h"
#include "distances.h"
#include "misc.h"
#include "options.h"
#include "parameter_tracer.h"
#include "particle.h"
#include "partition_functions.h"
#include "progress_reporter.h"
#include "resampler.h"

using namespace arma;

// [[Rcpp::export]]
Rcpp::List run_smc(
  Rcpp::List input_timeseries,
  Rcpp::List input_prior,
  Rcpp::List input_options,
  Rcpp::List input_sort_matrices,
  Rcpp::List input_sort_counts
) {

  Prior prior{input_prior};
  Options options{input_options};

  auto data = setup_data(input_timeseries, input_sort_matrices, input_sort_counts);
  auto pfun = choose_partition_function(prior.n_items, options.metric);
  auto particle_vector = create_particle_vector(options, prior, pfun);
  auto distfun = choose_distance_function(options.metric);
  auto resampler = choose_resampler(options.resampler);
  auto reporter = ProgressReporter(options.verbose);
  auto tracer = ParameterTracer(options.trace, options.trace_latent);
  Rcpp::IntegerVector n_particle_filters(data->n_timepoints());
  double log_marginal_likelihood{};

  int T = data->n_timepoints();
  vec ESS(T);
  ivec resampling = zeros<ivec>(T);

  std::vector<double> iteration_times;

  for(size_t t{}; t < T; t++) {
    reporter.report_time(t);

    for(auto& p : particle_vector) {
      p.run_particle_filter(t, prior, data, pfun, distfun, resampler,
                            options.latent_rank_proposal);
      p.log_importance_weight += p.log_incremental_likelihood(t);
      p.sample_particle_filter();
    }
    vec normalized_log_importance_weights = normalize_log_importance_weights(particle_vector);

    log_marginal_likelihood += log_marginal_likelihood_increment(
      particle_vector, normalized_log_importance_weights, t);

    ESS(t) = pow(norm(exp(normalized_log_importance_weights), 2), -2);
    reporter.report_ess(ESS(t));

    if(ESS(t) < options.resampling_threshold) {
      resampling(t) = 1;
      reporter.report_resampling();
      ivec new_counts = resampler->resample(
        normalized_log_importance_weights.size(),
        exp(normalized_log_importance_weights));

      particle_vector = update_vector(new_counts, particle_vector);
      vec alpha_sd = compute_alpha_stddev(particle_vector);

      size_t iter{};
      double accepted{};
      int n_unique_particles{};

      do {
        iter++;
        for(auto& p : particle_vector) {
          accepted += p.rejuvenate(t, options, prior, data, pfun, distfun, resampler, alpha_sd);
        }

        n_unique_particles = find_unique_alphas(particle_vector);
        reporter.report_rejuvenation(n_unique_particles);

      } while((2.0 * n_unique_particles < particle_vector.size()) && iter < options.max_rejuvenation_steps);



      std::for_each(particle_vector.begin(), particle_vector.end(),
                    [](Particle& p){ p.log_importance_weight = 0; });

      double acceptance_rate = accepted / particle_vector.size() / iter;
      reporter.report_acceptance_rate(acceptance_rate);

      if(acceptance_rate < options.doubling_threshold && options.n_particle_filters < options.max_particle_filters) {
        for(auto& p : particle_vector) {
          double log_Z_old = compute_log_Z(p.particle_filters, t);

          int S = p.particle_filters.size() * 2;
          ivec new_counts = resampler->resample(S, exp(p.log_normalized_particle_filter_weights));
          p.particle_filters = update_vector(new_counts, p.particle_filters);
          p.log_normalized_particle_filter_weights = Rcpp::NumericVector(S, -log(p.particle_filters.size()));

          double log_Z_new = compute_log_Z(p.particle_filters, t);
          p.log_importance_weight += log_Z_new - log_Z_old;
        }
        options.n_particle_filters *= 2;
        reporter.report_expansion(options.n_particle_filters);
      }
    }

    tracer.update_trace(particle_vector, t);
    n_particle_filters(t) = options.n_particle_filters;

  }

  mat alpha(prior.n_clusters, particle_vector.size());
  ucube rho(prior.n_items, prior.n_clusters, particle_vector.size());
  mat tau(prior.n_clusters, particle_vector.size());
  cube cluster_probabilities;
  if(prior.n_clusters > 1) {
    cluster_probabilities = cube(particle_vector.size(), particle_vector[0].particle_filters[0].cluster_probabilities.n_cols, prior.n_clusters);
  }

  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha.col(i) = particle_vector[i].parameters.alpha;
    rho.slice(i) = particle_vector[i].parameters.rho;
    tau.col(i) = particle_vector[i].parameters.tau;

    if(prior.n_clusters > 1) {
      cluster_probabilities.row(i) = particle_vector[i].particle_filters[particle_vector[i].conditioned_particle_filter].cluster_probabilities.t();
    }
  }

  return Rcpp::List::create(
    Rcpp::Named("alpha") = alpha,
    Rcpp::Named("rho") = rho,
    Rcpp::Named("tau") = tau,
    Rcpp::Named("cluster_probabilities") = cluster_probabilities,
    Rcpp::Named("ESS") = ESS,
    Rcpp::Named("resampling") = resampling,
    Rcpp::Named("n_particle_filters") = n_particle_filters,
    Rcpp::Named("importance_weights") = exp(normalize_log_importance_weights(particle_vector)),
    Rcpp::Named("log_marginal_likelihood") = log_marginal_likelihood,
    Rcpp::Named("alpha_traces") = tracer.alpha_traces,
    Rcpp::Named("rho_traces") = tracer.rho_traces,
    Rcpp::Named("tau_traces") = tracer.tau_traces,
    Rcpp::Named("log_importance_weights_traces") = tracer.log_importance_weights_traces,
    Rcpp::Named("latent_rankings_traces") = tracer.latent_rankings_traces
  );
}
