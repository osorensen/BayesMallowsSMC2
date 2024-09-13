#include "particle.h"
#include "resampler.h"
#include "setdiff.h"
#include "softmax.h"

using namespace arma;

void Particle::run_particle_filter(
    int t,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Resampler>& resampler,
    const std::unique_ptr<LatentProposer>& latent_proposer,
    const Prior& prior
) {
  if(data->type() == "CompleteRankings") {
    imat mat = data->get_timepoint_data(t);
    vec log_cluster_contribution = zeros(parameters.alpha.size());
    
    for(size_t c{}; c < log_cluster_contribution.size(); c++) {
      log_cluster_contribution(c) = log(parameters.tau(c)) - 
        pfun->logz(parameters.alpha(c)) - parameters.alpha(c) * 
        distfun->d(mat, parameters.rho.col(c));
    }
    
    double max_log = log_cluster_contribution.max();
    
    log_likelihood_increment(t) = 
      max_log + log(sum(exp(log_cluster_contribution - max_log)));
    for(auto& pf : particle_filters) {
      pf.log_weight(t) = log_likelihood_increment(t);
    }
    
    log_weight(t) += log_likelihood_increment(t);
    
    if(log_cluster_contribution.size() > 1) {
      vec log_cluster_probabilities = log_cluster_contribution - log_likelihood_increment(t);
      ivec new_cluster_labels = Rcpp::sample(
        log_cluster_probabilities.size(), mat.n_rows, true, 
        Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(exp(log_cluster_probabilities)))
      );
      
      uvec inds = data->get_users_at_timepoint(t);
      for(auto& pf : particle_filters) {
        pf.cluster_labels.elem(inds) = new_cluster_labels;
      }
    }
  } else if(data->type() == "PartialRankings") {
    
    if(t > 1) {
      vec log_weights = extract_weights(particle_filters, t - 1);
      vec normalized_weights = softmax(log_weights);
      ivec new_inds = resampler->resample(particle_filters.size(), normalized_weights);
      particle_filters = replace_elements(particle_filters, new_inds);
    }
    
    uvec users_at_timepoint = data->get_users_at_timepoint(t);
    uvec users_before_timepoint = data->get_users_before_timepoint(t);
    uvec new_users = setdiff(users_at_timepoint, users_before_timepoint);
    uvec updated_users = setdiff(users_at_timepoint, new_users);
    
    for(auto& pf : particle_filters) {
      double log_proposal_probability{};
      double updated_users_previous{};
      double new_and_updated_users{};
      
      for(int nu : new_users) {
        ivec obs = data->get_user_data(t, nu);
      
        auto prop = latent_proposer->propose(obs, prior);
        pf.latent_rankings = join_vert(
          pf.latent_rankings, 
          join_horiz(ivec{nu}, prop.proposal.t())
          );
        log_proposal_probability += prop.log_probability;
        new_and_updated_users += compute_log_likelihood_contribution(
          prop.proposal, distfun, pfun
        );
      }
      for(int uu : updated_users) {
        auto obs = data->get_user_data(t, uu);
        uvec observed_inds = find(obs > 0);
        
        ivec current_latent_ranking = pf.extract_latent_ranking(uu);
        bool consistent = 
          check_rank_consistency(current_latent_ranking, obs, observed_inds);
        
        if(!consistent) {
          updated_users_previous += compute_log_likelihood_contribution(
            current_latent_ranking, distfun, pfun
          );
          auto prop = latent_proposer->propose(obs, prior);
          pf.insert_latent_ranking(prop.proposal, uu);
          log_proposal_probability += prop.log_probability;
          
          new_and_updated_users += compute_log_likelihood_contribution(
            prop.proposal, distfun, pfun
          );
        }
      }
      
      log_likelihood_increment(t) = 
        new_and_updated_users - log_proposal_probability - updated_users_previous;
      log_weight(t) += log_likelihood_increment(t);
    }
    
    
  } else {
    Rcpp::stop("Not implemented yet.");
  }
}
