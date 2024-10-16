#include <algorithm>
#include <vector>
#include <Rmath.h>
#include "misc.h"
#include "particle.h"
#include "sample_latent_rankings.h"

using namespace arma;

StaticParameters::StaticParameters(const vec& alpha, const umat& rho, const vec& tau) :
  alpha { alpha }, rho { rho }, tau { tau } {}

StaticParameters::StaticParameters(const Prior& prior) :
  alpha { randg(prior.n_clusters, distr_param(prior.alpha_shape, 1 / prior.alpha_rate)) },
  rho { umat(prior.n_items, prior.n_clusters) },
  tau { normalise(randg(prior.n_clusters, distr_param(prior.cluster_concentration, 1)), 1) }
  {
    rho.each_col([&prior](uvec& a){ a = shuffle(regspace<uvec>(1, prior.n_items)); });
  }

Particle::Particle(const Options& options, const StaticParameters& parameters,
                   const std::unique_ptr<PartitionFunction>& pfun) :
  parameters { parameters },
  particle_filters(create_particle_filters(options)),
  log_normalized_particle_filter_weights (
      Rcpp::NumericVector(options.n_particle_filters, -log(options.n_particle_filters))
  ){
    logz = zeros(parameters.tau.size());
    for(size_t i{}; i < logz.size(); i++) {
      logz(i) = pfun->logz(parameters.alpha(i));
    }
  }

std::vector<std::pair<std::string, int>>::const_iterator findIntInPairs(const std::vector<std::pair<std::string, int>>& vec, int a) {
  return std::find_if(vec.begin(), vec.end(), [a](const std::pair<std::string, int>& element) {
    return element.second == a;
  });
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
    particle_filters = update_vector(new_inds, particle_filters);
    log_normalized_particle_filter_weights =
      Rcpp::NumericVector(particle_filters.size(), -log(particle_filters.size()));
  }

  for(auto& pf : particle_filters) {
    auto proposal = sample_latent_rankings(
      data, t, prior, latent_rank_proposal, parameters, pf.latent_rankings,
      pfun, distfun);

    if(prior.n_clusters > 1) {
      pf.cluster_assignments =
        join_cols(pf.cluster_assignments, proposal.cluster_assignment);
    }

    double log_prob{};

    for(size_t i{}; i < proposal.proposal.n_cols; i++) {
      vec log_cluster_contribution(prior.n_clusters);
      for(size_t c{}; c < prior.n_clusters; c++) {
        log_cluster_contribution(c) = log(parameters.tau(c)) - pfun->logz(parameters.alpha(c)) -
          parameters.alpha(c) * distfun->d(proposal.proposal.col(i), parameters.rho.col(c));
      }
      if(data->updated_users) {
        pf.user_delta[proposal.users[i]] =
          log_sum_exp(log_cluster_contribution) - proposal.log_probability(i);
      }

      log_prob += log_sum_exp(log_cluster_contribution);
    }
    // Subtract inconsistent users' old latent rankings
    umat proposal_new_users = proposal.proposal;
    uvec cols_to_drop{};
    for(auto uu = proposal.updated_inconsistent_users.begin();
        uu != proposal.updated_inconsistent_users.end(); ++uu) {
      unsigned int index_in_proposal = uu->second;
      unsigned int index_in_latent = data->observed_users[uu->first];

      log_prob -= pf.user_delta[uu->first];
      pf.latent_rankings.col(index_in_latent) = proposal.proposal.col(index_in_proposal);
      cols_to_drop = join_vert(cols_to_drop, uvec{index_in_proposal});
    }

    proposal_new_users.shed_cols(cols_to_drop);
    pf.latent_rankings = join_horiz(pf.latent_rankings, proposal_new_users);
    pf.log_weight.resize(t + 1);
    pf.log_weight(t) = log_prob - sum(proposal.log_probability);
  }

  vec log_pf_weights(log_normalized_particle_filter_weights.size());
  std::transform(
    particle_filters.cbegin(), particle_filters.cend(), log_pf_weights.begin(),
    [t](const ParticleFilter& pf){ return pf.log_weight(t); });

  log_incremental_likelihood.resize(log_incremental_likelihood.size() + 1);
  log_incremental_likelihood(log_incremental_likelihood.size() - 1) = log_mean_exp(log_pf_weights);
  log_normalized_particle_filter_weights = softmax(log_pf_weights);
}

void Particle::sample_particle_filter() {
  Rcpp::NumericVector probs = Rcpp::exp(log_normalized_particle_filter_weights);
  conditioned_particle_filter = Rcpp::sample(probs.size(), 1, false, probs, false)[0];
}

std::vector<Particle> create_particle_vector(const Options& options, const Prior& prior,
                                             const std::unique_ptr<PartitionFunction>& pfun) {
  std::vector<Particle> result;
  result.reserve(options.n_particles);

  for(size_t i{}; i < options.n_particles; i++) {
    result.push_back(Particle{options, StaticParameters(prior), pfun});
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

vec normalize_log_importance_weights(const std::vector<Particle>& particle_vector) {
  vec log_importance_weights(particle_vector.size());
  std::transform(
    particle_vector.cbegin(), particle_vector.cend(),
    log_importance_weights.begin(),
    [](const Particle& p) { return p.log_importance_weight; });

  return softmax(log_importance_weights);
}

  double log_marginal_likelihood_increment(
      const std::vector<Particle>& particle_vector,
      const vec& normalized_log_importance_weights,
      int t
  ) {
  vec unconditional_log_incremental(particle_vector.size());
  for(size_t i{}; i < particle_vector.size(); i++) {
    unconditional_log_incremental(i) =
      normalized_log_importance_weights(i) + particle_vector[i].log_incremental_likelihood(t);
  }
  return log_sum_exp(unconditional_log_incremental);
}

vec compute_alpha_stddev(const std::vector<Particle>& particle_vector) {
  mat alpha_values(particle_vector.size(), particle_vector[0].parameters.alpha.size());
  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha_values.row(i) = particle_vector[i].parameters.alpha.t();
  }
  return stddev(alpha_values, 0, 0);
}

double compute_log_Z(const std::vector<ParticleFilter>& pf, int max_time) {
  double log_Z{};
  for(size_t s{}; s < max_time + 1; s++) {
    vec log_weights(pf.size());
    std::transform(
      pf.begin(), pf.end(), log_weights.begin(),
      [s](const ParticleFilter& pf) { return pf.log_weight(s); });
    log_Z += log_mean_exp(log_weights);
  }
  return log_Z;
}
