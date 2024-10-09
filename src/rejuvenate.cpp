#include <algorithm>
#include <Rmath.h>
#include "misc.h"
#include "particle.h"
#include "sample_latent_rankings.h"

using namespace arma;

uvec leap_and_shift(const uvec& current_rho, unsigned int cluster, const Prior& prior) {
  unsigned int u = randi(distr_param(0, prior.n_items - 1));
  uvec intermediate_rho = current_rho;
  uvec rho_proposal = intermediate_rho;
  int rho_u = intermediate_rho(u);
  uvec support = setdiff(
    regspace<uvec>(std::max(1, rho_u - 1), std::min(prior.n_items, rho_u + 1)),
    uvec(rho_u)
  );

  unsigned int index = randi(distr_param(0, support.size() - 1));
  intermediate_rho(u) = support(index);
  for(size_t i{}; i < intermediate_rho.size(); i++) {
    if(current_rho(i) == current_rho(u)) {
      rho_proposal(u) = intermediate_rho(i);
    } else if(current_rho(u) < current_rho(i) &&
      current_rho(i) <= intermediate_rho(u) &&
      intermediate_rho(u) > current_rho(u)) {
      rho_proposal(i) = current_rho(i) - 1;
    } else if(current_rho(u) > current_rho(i) &&
      current_rho(i) >= intermediate_rho(u) &&
      intermediate_rho(u) < current_rho(u)) {
      rho_proposal(i) = current_rho(i) + 1;
    } else {
      rho_proposal(i) = current_rho(i);
    }
  }
  uvec v1 = unique(rho_proposal);
  uvec v2 = regspace<uvec>(1, prior.n_items);
  if(!approx_equal(v1, v2, "absdiff", 0)) {
    Rcpp::stop("Something wrong with rho proposal.");
  }

  return rho_proposal;
}


bool Particle::rejuvenate(
    unsigned int T, const Options& options, const Prior& prior,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<Resampler>& resampler,
    const vec& alpha_sd
) {
  vec alpha_proposal(prior.n_clusters);
  umat rho_proposal(prior.n_items, prior.n_clusters);

  for(size_t cluster{}; cluster < prior.n_clusters; cluster++) {
    alpha_proposal(cluster) = R::rlnorm(log(parameters.alpha(cluster)), std::max(.001, alpha_sd(cluster)));
    rho_proposal.col(cluster) = leap_and_shift(parameters.rho.col(cluster), cluster, prior);
  }

  Particle proposal_particle(options, prior);
  proposal_particle.parameters = StaticParameters{alpha_proposal, rho_proposal, parameters.tau};

  auto observed_users_backup = data->observed_users;
  data->observed_users.clear();

  for(size_t t{}; t < T + 1; t++) {
    proposal_particle.run_particle_filter(t, prior, data, pfun, distfun, resampler, options.latent_rank_proposal);
    proposal_particle.log_importance_weight += proposal_particle.log_incremental_likelihood(t);
    data->update_observed_users(t);
  }

  data->observed_users = std::move(observed_users_backup);

  Rcpp::NumericVector probs = Rcpp::exp(proposal_particle.log_normalized_particle_filter_weights);
  int proposed_particle_filter = Rcpp::sample(probs.size(), 1, false, probs, false)[0];

  vec additional_terms = prior.alpha_shape * (log(alpha_proposal) - log(parameters.alpha)) -
    prior.alpha_rate * (alpha_proposal - parameters.alpha);
  double log_ratio = proposal_particle.log_importance_weight -
    sum(this->log_incremental_likelihood) + accu(additional_terms);

  bool accepted{};
  if(log_ratio > log(randu())) {
    parameters = StaticParameters{alpha_proposal, rho_proposal, parameters.tau};
    this->conditioned_particle_filter = proposed_particle_filter;
    this->log_importance_weight = proposal_particle.log_importance_weight;
    this->log_incremental_likelihood = proposal_particle.log_incremental_likelihood;
    this->log_normalized_particle_filter_weights = proposal_particle.log_normalized_particle_filter_weights;
    accepted = true;
  } else {
    accepted = false;
  }

  uvec cluster_assignments = particle_filters[conditioned_particle_filter].cluster_assignments;
  uvec cluster_frequencies = hist(cluster_assignments, regspace<uvec>(0, prior.n_clusters - 1));

  for(size_t cluster{}; cluster < prior.n_clusters; cluster++) {
    parameters.tau(cluster) = R::rgamma(cluster_frequencies(cluster) + prior.cluster_concentration, 1.0);
  }
  parameters.tau = normalise(parameters.tau, 1);
  sample_particle_filter();

  return accepted;
}
