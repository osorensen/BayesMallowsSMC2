#include <algorithm>
#include <Rmath.h>
#include "misc.h"
#include "particle.h"
#include "sample_latent_rankings.h"
#include "sample_cluster_assignments.h"
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
    unsigned int t, const Prior& prior, const std::unique_ptr<Data>& data,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun) {
  // resample
  if(t > 0) {
    Rcpp::NumericVector probs = exp(log_normalized_particle_filter_weights);
    ivec new_inds = Rcpp::sample(probs.size(), probs.size(), true, probs, false);
    std::vector<ParticleFilter> tmp(particle_filters.size());
    for(size_t i{}; i < new_inds.size(); i++) {
      tmp[i] = particle_filters[new_inds[i]];
    }
    particle_filters = tmp;
    std::fill(log_normalized_particle_filter_weights.begin(),
              log_normalized_particle_filter_weights.end(), -log(particle_filters.size()));
  }

  for(auto& pf : particle_filters) {
    // sample latent rankings
    auto proposal = sample_latent_rankings(data, t, prior);
    pf.latent_rankings = proposal.proposal;

    // sample cluster indicators
    uvec new_cluster_assignments = sample_cluster_assignments(pf.latent_rankings, parameters, pfun, distfun);
    pf.cluster_assignments = join_cols(pf.cluster_assignments, new_cluster_assignments);

    // compute weights
    vec log_cluster_contribution(prior.n_clusters);

    for(size_t c{}; c < prior.n_clusters; c++) {
      unsigned int total_distance{};
      for(size_t i{}; i < pf.latent_rankings.n_cols; i++) {
        total_distance += distfun->d(pf.latent_rankings.col(i), parameters.rho.col(c));
      }
      log_cluster_contribution(c) = log(parameters.tau(c)) - pfun->logz(parameters.alpha(c)) -
        parameters.alpha(c) / prior.n_items * total_distance;
    }
    double maxval = log_cluster_contribution.max();
    double log_prob = maxval + log(accu(exp(log_cluster_contribution - maxval)));
    pf.log_weight = log_prob - proposal.log_probability;
  }

  // normalize weights
  Rcpp::NumericVector tmp_pf_weights(log_normalized_particle_filter_weights.size());
  std::transform(
    particle_filters.cbegin(), particle_filters.cend(), tmp_pf_weights.begin(),
    [](const ParticleFilter& pf){ return pf.log_weight; });
  double maxval = Rcpp::max(tmp_pf_weights);

  log_incremental_likelihood.resize(log_incremental_likelihood.size() + 1);
  log_incremental_likelihood(log_incremental_likelihood.size() - 1) =
    maxval + log(mean(exp(tmp_pf_weights - maxval)));

  log_normalized_particle_filter_weights =
    tmp_pf_weights - (maxval + log(sum(exp(tmp_pf_weights - maxval))));
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

void Particle::rejuvenate(
    unsigned int t, const Prior& prior, const std::unique_ptr<Data>& data,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun,
    const vec& alpha_sd
) {
  // sample a complete cluster assignment and compute frequencies
  int pf_index = randi(distr_param(0, particle_filters.size() - 1));
  uvec cluster_assignments = particle_filters[pf_index].cluster_assignments;
  uvec cluster_frequencies = hist(cluster_assignments, regspace<uvec>(0, prior.n_clusters - 1));

  // sample proposals
  vec alpha_proposal(prior.n_clusters);
  vec tau_proposal(prior.n_clusters);
  umat rho_proposal(prior.n_items, prior.n_clusters);
  for(size_t cluster{}; cluster < prior.n_clusters; cluster++) {
    alpha_proposal(cluster) = R::rlnorm(log(parameters.alpha(cluster)), std::max(.001, alpha_sd(cluster)));
    tau_proposal(cluster) = R::rgamma(cluster_frequencies(cluster) + prior.cluster_concentration, 1.0);

    unsigned int u = randi(distr_param(0, prior.n_items - 1));
    Rcpp::Rcout << "u = " << u << std::endl;
    uvec intermediate_rho = parameters.rho.col(cluster);
    int rho_u = intermediate_rho(u);
    Rcpp::Rcout << "rho_u = " << rho_u << std::endl;
    uvec support = setdiff(
      regspace<uvec>(std::max(1, rho_u - 1), std::min(prior.n_items, rho_u + 1)),
      uvec(rho_u)
    );
    Rcpp::Rcout << "support = " << support << std::endl;
    unsigned int index = randi(distr_param(0, support.size() - 1));
    intermediate_rho(u) = support(index);
    for(size_t i{}; i < intermediate_rho.size(); i++) {
      if(parameters.rho(i, cluster) == parameters.rho(u, cluster)) {
        rho_proposal(u, cluster) = intermediate_rho(i);
      } else if(parameters.rho(u, cluster) < parameters.rho(i, cluster) &&
        parameters.rho(i, cluster) <= intermediate_rho(u) &&
        intermediate_rho(u) > parameters.rho(u, cluster)) {
        rho_proposal(i, cluster) = parameters.rho(i, cluster) - 1;
      } else if(parameters.rho(u, cluster) > parameters.rho(i, cluster) &&
        parameters.rho(i, cluster) >= intermediate_rho(u) &&
        intermediate_rho(u) < parameters.rho(u, cluster)) {
        rho_proposal(i, cluster) = parameters.rho(i, cluster) + 1;
      } else {
        rho_proposal(i, cluster) = parameters.rho(i, cluster);
      }
    }
    uvec v1 = unique(rho_proposal.col(cluster));
    uvec v2 = regspace<uvec>(1, prior.n_items);
    if(!approx_equal(v1, v2, "absdiff", 0)) {
      Rcpp::stop("Something wrong with rho proposal.");
    }
  }
  tau_proposal = normalise(tau_proposal, 1);

}
