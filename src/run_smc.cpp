// [[Rcpp::depends(RcppArmadillo)]]
#include <RcppArmadillo.h>
#include <numeric>
#include "prior.h"
#include "data.h"
#include "particle.h"
#include "options.h"
#include "partition_functions.h"
#include "distances.h"
#include "resampler.h"
#include "progress_reporter.h"
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

  int T = data->n_timepoints();
  mat alpha_trace(prior.n_clusters, T);
  for(size_t t{}; t < T; t++) {
    reporter.report_time(t);
    data->update_topological_sorts(t, prior.n_items);
    for(auto& p : particle_vector) {
      p.run_particle_filter(t, prior, data, pfun, distfun, resampler);
      p.log_importance_weight += p.log_incremental_likelihood(t);
    }

    vec log_importance_weights(particle_vector.size());
    std::transform(
      particle_vector.cbegin(), particle_vector.cend(),
      log_importance_weights.begin(),
      [](const Particle& p) { return p.log_importance_weight; });

    vec log_normalized_importance_weights =
      log_importance_weights - (max(log_importance_weights) +
      log(sum(exp(log_importance_weights - max(log_importance_weights)))));

    double ess = pow(norm(exp(log_normalized_importance_weights), 2), -2);
    reporter.report_ess(ess);

    if(ess < options.resampling_threshold) {
      reporter.report_resampling();
      ivec new_inds = resampler->resample(exp(log_normalized_importance_weights));
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
      vec alpha0_tmp = vec(particle_vector.size());
      do {
        iter++;
        for(size_t i{}; i < particle_vector.size(); i++) {
          particle_vector[i].rejuvenate(t, options, prior, data, pfun, distfun, resampler, alpha_sd.t());
        }
        std::transform(
          particle_vector.cbegin(), particle_vector.cend(), alpha0_tmp.begin(),
          [](const Particle& p) { return p.parameters.alpha(0); });

        unique_particles = find_unique(alpha0_tmp);
        n_unique_particles = unique_particles.size();
        reporter.report_rejuvenation(n_unique_particles);

      } while((2.0 * n_unique_particles < particle_vector.size()) && iter < 20);

      std::for_each(particle_vector.begin(), particle_vector.end(),
                    [](Particle& p) { p.log_importance_weight = 1; });
    }

    for(size_t i{}; i < particle_vector.size(); i++) {
      alpha_trace.col(t) += particle_vector[i].parameters.alpha;
    }
    alpha_trace.col(t) /= particle_vector.size();
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
    Rcpp::Named("alpha_trace") = alpha_trace,
    Rcpp::Named("alpha") = alpha,
    Rcpp::Named("rho") = rho,
    Rcpp::Named("tau") = tau
  );
}
