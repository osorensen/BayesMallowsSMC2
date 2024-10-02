// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include <numeric>
#include <limits>
#include "prior.h"
#include "data.h"
#include "particle.h"
#include "options.h"
#include "partition_functions.h"
#include "distances.h"
#include "resampler.h"
#include "progress_reporter.h"
#include "parameter_tracer.h"
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
  auto resampler = choose_resampler(options.resampler);
  auto reporter = ProgressReporter(options.verbose);
  auto tracer = ParameterTracer(options.trace, options.trace_latent,
                                options.trace_directory);
  Rcpp::IntegerVector n_particle_filters(data->n_timepoints());
  double log_marginal_likelihood{0};

  int T = data->n_timepoints();
  for(size_t t{}; t < T; t++) {
    reporter.report_time(t);

    for(auto& p : particle_vector) {
      p.run_particle_filter(t, prior, data, pfun, distfun, resampler,
                            options.latent_rank_proposal);
      p.log_importance_weight += p.log_incremental_likelihood(t);
    }

    vec normalized_importance_weights = normalize_importance_weights(particle_vector);

    double log_marginal_likelihood_contribution = -std::numeric_limits<double>::infinity();
    vec log_incremental_likelihoods(normalized_importance_weights.size());

    for(size_t i{}; i < normalized_importance_weights.size(); i++) {
      log_incremental_likelihoods(i) =
        log(normalized_importance_weights(i)) + particle_vector[i].log_incremental_likelihood(t);
    }

    double max_log_incremental_likelihood = log_incremental_likelihoods.max();
    double sum_exp_term = 0.0;
    for(size_t i{}; i < normalized_importance_weights.size(); i++) {
      sum_exp_term += exp(log_incremental_likelihoods(i) - max_log_incremental_likelihood);
    }
    log_marginal_likelihood_contribution = max_log_incremental_likelihood + log(sum_exp_term);
    log_marginal_likelihood += log_marginal_likelihood_contribution;

    double ess = pow(norm(normalized_importance_weights, 2), -2);
    reporter.report_ess(ess);

    if(ess < options.resampling_threshold) {
      reporter.report_resampling();
      ivec new_inds = resampler->resample(normalized_importance_weights.size(),
                                          normalized_importance_weights);
      uvec unique_particles = find_unique(new_inds);
      int n_unique_particles = unique_particles.size();

      std::vector<Particle> tmp = particle_vector;
      for(size_t i{}; i < new_inds.size(); i++) {
        particle_vector[i] = tmp[new_inds[i]];
      }

      mat alpha_values(particle_vector.size(), prior.n_clusters);
      for(size_t i{}; i < particle_vector.size(); i++) {
        alpha_values.row(i) = particle_vector[i].parameters.alpha.t();
      }
      auto alpha_sd = stddev(alpha_values, 0, 0);

      size_t iter{};
      double accepted{};
      vec alpha0_tmp = vec(particle_vector.size());
      do {
        iter++;
        for(size_t i{}; i < particle_vector.size(); i++) {
          accepted += particle_vector[i].rejuvenate(
            t, options, prior, data, pfun, distfun, resampler, alpha_sd.t());
        }
        std::transform(
          particle_vector.cbegin(), particle_vector.cend(), alpha0_tmp.begin(),
          [](const Particle& p) { return p.parameters.alpha(0); });

        unique_particles = find_unique(alpha0_tmp);
        n_unique_particles = unique_particles.size();
        reporter.report_rejuvenation(n_unique_particles);

      } while((2.0 * n_unique_particles < particle_vector.size()) && iter < options.max_rejuvenation_steps);

      std::for_each(particle_vector.begin(), particle_vector.end(),
                    [](Particle& p) { p.log_importance_weight = 1; });

      double acceptance_rate = accepted / particle_vector.size() / iter;
      reporter.report_acceptance_rate(acceptance_rate);

      if(acceptance_rate < 0.2 && options.n_particle_filters < options.max_particle_filters) {
        for(auto& p : particle_vector) {
          auto tmp = p.particle_filters;
          int S = tmp.size() * 2;
          p.particle_filters.clear();
          p.particle_filters.reserve(S);
          ivec new_inds = resampler->resample(S, exp(p.log_normalized_particle_filter_weights));
          for(auto ni : new_inds) {
            p.particle_filters.push_back(tmp[ni]);
          }
          p.log_normalized_particle_filter_weights = Rcpp::NumericVector(S, -log(p.particle_filters.size()));

          double Z_old{};
          for(const auto& pf : tmp) {
            double max = pf.log_weight.max();
            Z_old += exp(max) * sum(exp(pf.log_weight - max));
          }
          Z_old /= tmp.size();
          double Z_new{};
          for(const auto& pf : p.particle_filters) {
            double max = pf.log_weight.max();
            Z_new += exp(max) * sum(exp(pf.log_weight - max));
          }
          Z_new /= p.particle_filters.size();

          p.log_importance_weight += log(Z_new) - log(Z_old);

        }
        options.n_particle_filters *= 2;
        reporter.report_expansion(options.n_particle_filters);
      }
    }

    data->update_observed_users(t);
    tracer.update_trace(particle_vector, t);
    n_particle_filters(t) = options.n_particle_filters;
  }

  mat alpha(prior.n_clusters, particle_vector.size());
  ucube rho(prior.n_items, prior.n_clusters, particle_vector.size());
  mat tau(prior.n_clusters, particle_vector.size());
  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha.col(i) = particle_vector[i].parameters.alpha;
    rho.slice(i) = particle_vector[i].parameters.rho;
    tau.col(i) = particle_vector[i].parameters.tau;
  }

  return Rcpp::List::create(
    Rcpp::Named("alpha") = alpha,
    Rcpp::Named("rho") = rho,
    Rcpp::Named("tau") = tau,
    Rcpp::Named("n_particle_filters") = n_particle_filters,
    Rcpp::Named("importance_weights") = normalize_importance_weights(particle_vector),
    Rcpp::Named("log_marginal_likelihood") = log_marginal_likelihood
  );
}
