#include "leap_and_shift.h"
#include "particle.h"
#include "softmax.h"

using namespace arma;

bool Particle::rejuvenate(
    int T,
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
  Particle proposal_particle{prior, smc_options, model_options, data};
  proposal_particle.alpha = R::rlnorm(log(alpha), std::max(.001, alpha_summaries.alpha_sd));
  proposal_particle.rho = leap_and_shift(rho);
  
  for(size_t t{}; t < T + 1; t++) {
    proposal_particle.run_particle_filter(
      t, data, distfun, pfun, resampler, latent_proposer);
  }
  
  double log_Z_proposal = sum(proposal_particle.log_likelihood_increment);
  double log_Z_current = sum(log_likelihood_increment);
  
  Rcpp::Rcout << "log_Z_proposal " << log_Z_proposal << std::endl
              << "log_Z_current " << log_Z_current << std::endl;
  
  double log_MH_ratio = log_Z_proposal - log_Z_current +
    sum(
      prior.alpha_shape * 
        log(proposal_particle.alpha / alpha) -
        prior.alpha_rate * (proposal_particle.alpha - alpha)
    );
  
  bool accept = log_MH_ratio > log(randu());
  if(accept) {
    alpha = proposal_particle.alpha;
    rho = proposal_particle.rho;
  }
  
  return accept;
}
