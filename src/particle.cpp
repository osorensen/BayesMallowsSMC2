#include <algorithm>
#include <Rmath.h>
#include "misc.h"
#include "particle.h"
#include "sample_latent_rankings.h"

using namespace arma;

Particle::Particle(const Options& options, const Prior& prior) :
  parameters { randg(prior.n_clusters, distr_param(prior.alpha_shape, 1 / prior.alpha_rate)),
               umat(prior.n_items, prior.n_clusters),
               normalise(randg(prior.n_clusters, distr_param(prior.cluster_concentration, 1)), 1) },
  particle_filters(create_particle_filters(options)),
  log_normalized_particle_filter_weights (
      Rcpp::NumericVector(options.n_particle_filters, -log(options.n_particle_filters))
  )
  {
    parameters.rho.each_col([&prior](uvec& a){ a = shuffle(regspace<uvec>(1, prior.n_items)); });
  }

void Particle::run_particle_filter(
    unsigned int t, const Prior& prior,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<Resampler>& resampler,
    std::string latent_rank_proposal) {

  if(t > 0) {
    ivec new_inds = resampler->resample(
      particle_filters.size(), exp(log_normalized_particle_filter_weights));
    std::vector<ParticleFilter> tmp(particle_filters.size());
    std::transform(new_inds.begin(), new_inds.end(), tmp.begin(),
                   [&](int index) { return particle_filters[index]; });
    particle_filters = std::move(tmp);
    log_normalized_particle_filter_weights =
      Rcpp::NumericVector(particle_filters.size(), -log(particle_filters.size()));
  }

  for(auto& pf : particle_filters) {
    auto proposal = sample_latent_rankings(data, t, prior, latent_rank_proposal,
                                           parameters, pf.latent_rankings,
                                           pfun, distfun);

    pf.cluster_assignments =
      join_cols(pf.cluster_assignments, proposal.cluster_assignment);

    double log_prob{};

    for(size_t i{}; i < proposal.proposal.n_cols; i++) {
      vec log_cluster_contribution(prior.n_clusters);
      for(size_t c{}; c < prior.n_clusters; c++) {
        log_cluster_contribution(c) = log(parameters.tau(c)) - pfun->logz(parameters.alpha(c)) -
          parameters.alpha(c) * distfun->d(proposal.proposal.col(i), parameters.rho.col(c));

        if(i < proposal.updated_inconsistent_users.size()) {
          auto it = std::find(data->observed_users.begin(), data->observed_users.end(),
                              proposal.updated_inconsistent_users[i]);
          int uiu_index = std::distance(data->observed_users.begin(), it);

          log_cluster_contribution(c) -= log(parameters.tau(c)) - pfun->logz(parameters.alpha(c)) -
            parameters.alpha(c) * distfun->d(pf.latent_rankings.col(uiu_index), parameters.rho.col(c));
        }
      }

      double maxval = log_cluster_contribution.max();
      log_prob += maxval + log(accu(exp(log_cluster_contribution - maxval)));
    }

    pf.log_weight.resize(t + 1);
    pf.log_weight(t) = log_prob - proposal.log_probability;

    for(const auto & uiu : proposal.updated_inconsistent_users) {
      auto it = std::find(data->observed_users.begin(), data->observed_users.end(), uiu);
      int uiu_index = std::distance(data->observed_users.begin(), it);

      auto propit = std::find(proposal.updated_inconsistent_users.begin(),
                              proposal.updated_inconsistent_users.end(), uiu);
      int prop_index = std::distance(proposal.updated_inconsistent_users.begin(), propit);

      pf.latent_rankings.col(uiu_index) = proposal.proposal.col(prop_index);
      proposal.proposal.shed_col(prop_index);
    }

    pf.latent_rankings = join_horiz(pf.latent_rankings, proposal.proposal);
  }

  Rcpp::NumericVector tmp_pf_weights(log_normalized_particle_filter_weights.size());
  std::transform(
    particle_filters.cbegin(), particle_filters.cend(), tmp_pf_weights.begin(),
    [t](const ParticleFilter& pf){ return pf.log_weight(t); });
  double maxval = Rcpp::max(tmp_pf_weights);

  log_incremental_likelihood.resize(log_incremental_likelihood.size() + 1);
  log_incremental_likelihood(log_incremental_likelihood.size() - 1) =
    maxval - std::log(tmp_pf_weights.size()) + log(sum(exp(tmp_pf_weights - maxval)));

  log_normalized_particle_filter_weights =
    tmp_pf_weights - (maxval + log(sum(exp(tmp_pf_weights - maxval))));
}

void Particle::sample_particle_filter() {
  Rcpp::NumericVector probs = Rcpp::exp(log_normalized_particle_filter_weights);
  conditioned_particle_filter = Rcpp::sample(probs.size(), 1, false, probs, false)[0];
}

std::vector<Particle> create_particle_vector(const Options& options, const Prior& prior) {
  std::vector<Particle> result;
  result.reserve(options.n_particles);

  for(size_t i{}; i < options.n_particles; i++) {
    result.push_back(Particle{options, prior});
  }

  return result;
}

std::vector<ParticleFilter> create_particle_filters(const Options& options) {
  std::vector<ParticleFilter> result;
  result.reserve(options.n_particle_filters);
  for(size_t i{}; i < options.n_particle_filters; i++) {
    result.push_back(ParticleFilter{});
  }

  return result;
}

vec normalize_importance_weights(const std::vector<Particle>& particle_vector) {
  vec log_importance_weights(particle_vector.size());
  std::transform(
    particle_vector.cbegin(), particle_vector.cend(),
    log_importance_weights.begin(),
    [](const Particle& p) { return p.log_importance_weight; });

  vec normalized_log_importance_weights =
    log_importance_weights - (max(log_importance_weights) +
    log(sum(exp(log_importance_weights - max(log_importance_weights)))));

  return exp(normalized_log_importance_weights);
}
