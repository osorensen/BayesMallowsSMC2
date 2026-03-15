#include "particle.h"
#include "misc.h"
#include "sample_latent_rankings.h"
#include <RcppArmadillo.h>
#include <algorithm>
#include <vector>

using namespace arma;

using namespace arma;

StaticParameters::StaticParameters(const vec &alpha, const umat &rho,
                                   const vec &tau)
    : alpha{alpha}, rho{rho}, tau{tau} {}

StaticParameters::StaticParameters(const Prior &prior)
    : alpha{Rcpp::rgamma(prior.n_clusters, prior.alpha_shape,
                         1 / prior.alpha_rate)},
      rho{umat(prior.n_items, prior.n_clusters)},
      tau{normalise(Rcpp::as<vec>(Rcpp::rgamma(prior.n_clusters,
                                               prior.cluster_concentration, 1)),
                    1)} {
  rho.each_col([&prior](uvec &a) {
    a = Rcpp::as<uvec>(Rcpp::sample(prior.n_items, prior.n_items, false));
  });
}

Particle::Particle(const Options &options, const StaticParameters &parameters,
                   const std::unique_ptr<PartitionFunction> &pfun)
    : parameters{parameters},
      particle_filters(create_particle_filters(options)),
      log_normalized_particle_filter_weights(Rcpp::NumericVector(
          options.n_particle_filters, -log(options.n_particle_filters))) {
  logz = zeros(parameters.tau.size());
  for (size_t i{}; i < logz.size(); i++) {
    logz(i) = pfun->logz(parameters.alpha(i));
  }
}

void Particle::run_particle_filter(
    unsigned int t, const Prior &prior, const std::unique_ptr<Data> &data,
    const std::unique_ptr<PartitionFunction> &pfun,
    const std::unique_ptr<Distance> &distfun,
    const std::unique_ptr<Resampler> &resampler,
    std::string latent_rank_proposal, bool conditional) {

  if (t > 0) {
    ivec new_counts = resampler->resample(
        conditional ? particle_filters.size() - 1 : particle_filters.size(),
        exp(log_normalized_particle_filter_weights));
    if (conditional)
      new_counts(0) += 1;
    particle_filters = update_vector(new_counts, particle_filters);
  }

  unsigned int pf_index{};
  for (auto &pf : particle_filters) {
    auto proposal = sample_latent_rankings(data, t, prior, latent_rank_proposal,
                                           parameters, pfun, distfun);

    if (conditional && pf_index == 0) {
      proposal.proposal = particle_filters[0].latent_rankings.col(t);
    }

    double log_prob{};

    for (size_t i{}; i < proposal.proposal.n_cols; i++) {
      vec log_cluster_contribution(prior.n_clusters);
      for (size_t c{}; c < prior.n_clusters; c++) {
        log_cluster_contribution(c) =
            log(parameters.tau(c)) - this->logz(c) -
            parameters.alpha(c) *
                distfun->d(proposal.proposal.col(i), parameters.rho.col(c));
      }
      log_prob += log_sum_exp(log_cluster_contribution);
    }

    pf.cluster_probabilities =
        join_horiz(pf.cluster_probabilities, proposal.cluster_probabilities);

    if (!(conditional && pf_index == 0)) {
      if (prior.n_clusters > 1) {
        pf.index = join_cols(pf.index, uvec{pf_index});
        pf.cluster_assignments =
            join_cols(pf.cluster_assignments, proposal.cluster_assignment);
      }
      pf.latent_rankings = join_horiz(pf.latent_rankings, proposal.proposal);
    }

    pf.log_weight.resize(t + 1);
    pf.log_weight(t) = log_prob - sum(proposal.log_probability);
    pf_index++;
  }

  vec log_pf_weights(log_normalized_particle_filter_weights.size());
  std::transform(particle_filters.cbegin(), particle_filters.cend(),
                 log_pf_weights.begin(),
                 [t](const ParticleFilter &pf) { return pf.log_weight(t); });

  log_incremental_likelihood.resize(log_incremental_likelihood.size() + 1);
  log_incremental_likelihood(log_incremental_likelihood.size() - 1) =
      log_mean_exp(log_pf_weights);
  log_normalized_particle_filter_weights = softmax(log_pf_weights);

  if (stored_weights.size() <= t) {
    stored_weights.push_back(exp(log_normalized_particle_filter_weights));
  } else {
    stored_weights[t] = exp(log_normalized_particle_filter_weights);
  }
}

void Particle::assemble_backward_trajectory(
    unsigned int T, const std::unique_ptr<Resampler> &resampler) {
  // We need to assemble a new reference trajectory traversing backwards from T
  // to 0. The independence property means the transition density factors out of
  // backward weights. Thus B_t is simply drawn from W_t independently.

  ParticleFilter new_reference;
  new_reference.log_weight.resize(T + 1);

  // Note: cluster_probabilities has size [cluster x (number of users up to T)]
  // We need to build these up. Actually, they are built horizontally (joined).
  // So we insert columns at the beginning.

  for (int t = T; t >= 0; --t) {
    arma::vec current_weights = stored_weights[t];

    // Sample a single index b_t based on current_weights
    arma::ivec counts = resampler->resample(1, current_weights);
    unsigned int b_t =
        arma::as_scalar(arma::find(counts > 0, 1)); // The chosen index

    unsigned int num_users_at_t =
        particle_filters[b_t]
            .latent_rankings.col(t)
            .n_cols; // actually wait, .col(t) returns EXACTLY 1 column.

    if (new_reference.latent_rankings.is_empty()) {
      new_reference.latent_rankings =
          particle_filters[b_t].latent_rankings.col(t);
      if (parameters.tau.size() > 1) {
        // The total number of users up to time t in the forward pass is the
        // length of cluster_assignments
        unsigned int end_idx =
            particle_filters[b_t].cluster_assignments.n_elem - 1;
        // Since .col(t) grabbed 1 column, but what if multiple users were
        // processed? Ah! In `run_particle_filter`, `proposal.proposal` is
        // joined! Wait, `pf.latent_rankings = join_horiz(pf.latent_rankings,
        // proposal.proposal);` If `proposal.proposal` had 5 columns at time
        // `t`, then `pf.latent_rankings` grew by 5 columns! So
        // `latent_rankings` columns correspond to USERS, not timepoints! So
        // `col(t)` is completely wrong! We need to extract the columns
        // corresponding to time `t`. Let's look at `sample_latent_rankings`.
        // For complete data, 1 user = 1 row = 1 timepoint! Wait... for mixture
        // models, see test: `compute_sequentially(mixtures[1:50,])`. `mixtures`
        // has 1 row per user. So `n_timepoints` = 50. At each timepoint, 1 user
        // is processed. So `proposal.proposal.n_cols` = 1. Thus
        // `latent_rankings` has exactly 1 column per timepoint.
        // `num_users_at_t` is always 1! SO WHY DID IT SEGFAULT? Because
        // `col(t)` returns exactly 1 column, `num_users_at_t` is 1. Let's check
        // `start_idx`.
        new_reference.cluster_assignments =
            particle_filters[b_t].cluster_assignments.subvec(t, t);
        new_reference.cluster_probabilities =
            particle_filters[b_t].cluster_probabilities.cols(t, t);
        new_reference.index = uvec(T + 1, fill::zeros);
      }
    } else {
      new_reference.latent_rankings.insert_cols(
          0, particle_filters[b_t].latent_rankings.col(t));
      if (parameters.tau.size() > 1) {
        new_reference.cluster_assignments.insert_rows(
            0, particle_filters[b_t].cluster_assignments.subvec(t, t));
        new_reference.cluster_probabilities.insert_cols(
            0, particle_filters[b_t].cluster_probabilities.cols(t, t));
      }
    }

    new_reference.log_weight(t) = particle_filters[b_t].log_weight(t);
  }

  this->particle_filters[0] = new_reference;
  this->conditioned_particle_filter = 0;
}

void Particle::sample_particle_filter() {
  Rcpp::NumericVector probs = Rcpp::exp(log_normalized_particle_filter_weights);
  conditioned_particle_filter =
      Rcpp::sample(probs.size(), 1, false, probs, false)[0];
}

std::vector<Particle>
create_particle_vector(const Options &options, const Prior &prior,
                       const std::unique_ptr<PartitionFunction> &pfun) {
  std::vector<Particle> result;
  result.reserve(options.n_particles);

  for (size_t i{}; i < options.n_particles; i++) {
    result.push_back(Particle{options, StaticParameters(prior), pfun});
  }

  return result;
}

std::vector<ParticleFilter> create_particle_filters(const Options &options) {
  std::vector<ParticleFilter> result;
  result.reserve(options.n_particle_filters);
  for (size_t i{}; i < options.n_particle_filters; i++) {
    result.push_back(ParticleFilter{});
  }

  return result;
}

vec normalize_log_importance_weights(
    const std::vector<Particle> &particle_vector) {
  vec log_importance_weights(particle_vector.size());
  std::transform(particle_vector.cbegin(), particle_vector.cend(),
                 log_importance_weights.begin(),
                 [](const Particle &p) { return p.log_importance_weight; });

  return softmax(log_importance_weights);
}

double
log_marginal_likelihood_increment(const std::vector<Particle> &particle_vector,
                                  const vec &normalized_log_importance_weights,
                                  int t) {
  vec unconditional_log_incremental(particle_vector.size());
  for (size_t i{}; i < particle_vector.size(); i++) {
    unconditional_log_incremental(i) =
        normalized_log_importance_weights(i) +
        particle_vector[i].log_incremental_likelihood(t);
  }
  return log_sum_exp(unconditional_log_incremental);
}

vec compute_alpha_stddev(const std::vector<Particle> &particle_vector) {
  mat alpha_values(particle_vector.size(),
                   particle_vector[0].parameters.alpha.size());
  for (size_t i{}; i < particle_vector.size(); i++) {
    alpha_values.row(i) = particle_vector[i].parameters.alpha.t();
  }
  return stddev(alpha_values, 0, 0).t();
}

double compute_log_Z(const std::vector<ParticleFilter> &pf, int max_time) {
  double log_Z{};
  for (size_t s{}; s < max_time + 1; s++) {
    vec log_weights(pf.size());
    std::transform(pf.begin(), pf.end(), log_weights.begin(),
                   [s](const ParticleFilter &pf) { return pf.log_weight(s); });
    log_Z += log_mean_exp(log_weights);
  }
  return log_Z;
}
