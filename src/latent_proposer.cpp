#include "latent_proposer.h"
#include "setdiff.h"
using namespace arma;

LatentRankProposal Uniform::propose(const arma::ivec& ranking) {
  int n_items = ranking.size();
  ivec proposal = ranking;
  uvec observed_inds = find(ranking > 0);
  uvec missing_inds = find(ranking == 0);
  ivec available_rankings = setdiff<ivec>(
    regspace<ivec>(1, 1, n_items), 
    ranking(observed_inds)
    );
  
  proposal(missing_inds) = shuffle(available_rankings);
  return LatentRankProposal{proposal, -lgamma(missing_inds.size() + 1)};
}

std::unique_ptr<LatentProposer> choose_latent_proposer(const SMCOptions& smc_options) {
  if(smc_options.latent_rank_proposer == "uniform") {
    return std::make_unique<Uniform>();
  } else {
    Rcpp::stop("Unknown latent rank proposer.");
  }
}