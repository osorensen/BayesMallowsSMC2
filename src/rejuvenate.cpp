#include "particle.h"
#include "softmax.h"

using namespace arma;

bool Particle::rejuvenate(
    int t,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Resampler>& resampler,
    const std::unique_ptr<LatentProposer>& latent_proposer,
    const Prior& prior,
    const SMCOptions& smc_options,
    const ModelOptions& model_options,
    const AlphaSummaries& alpha_summaries
) {
  vec pf_log_weights = extract_weights(particle_filters, t);
  vec normalized_pf_weights = softmax(pf_log_weights);
  
  Rcpp::NumericVector probs = 
    Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(normalized_pf_weights));
  int k = Rcpp::sample(probs.size(), 1, true, probs, false)[0];
  
  Particle proposal_particle{smc_options, model_options, data, alpha_summaries,
                             parameters};
  
  for(size_t t_index{}; t_index < t + 1; t_index++) {
    proposal_particle.run_particle_filter(
      t_index, data, distfun, pfun, resampler, latent_proposer);
  }
  
  double log_Z_proposal = sum(proposal_particle.log_likelihood_increment);
  double log_Z_current = sum(log_likelihood_increment);
  
  vec pf_proposal_log_weights = 
    extract_weights(proposal_particle.particle_filters, t);
  vec normalized_proposal_weights = softmax(pf_proposal_log_weights);
  
  Rcpp::NumericVector proposal_probs = 
    Rcpp::as<Rcpp::NumericMatrix>(Rcpp::wrap(normalized_proposal_weights));
  int k_proposal = Rcpp::sample(
    proposal_probs.size(), 1, true, proposal_probs, false)[0];
  
  double log_MH_ratio = log_Z_proposal - log_Z_current +
    sum(
      prior.alpha_shape * 
        log(proposal_particle.parameters.alpha / parameters.alpha) -
        prior.alpha_rate * 
        (proposal_particle.parameters.alpha - parameters.alpha)
    );
  
  bool accept = log_MH_ratio > log(randu());
  if(accept) {
    parameters.alpha = proposal_particle.parameters.alpha;
    parameters.rho = proposal_particle.parameters.rho;
    k = k_proposal;
  }
  
  return accept;
}
