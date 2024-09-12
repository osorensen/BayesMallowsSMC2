#include "particle.h"

using namespace arma;

void Particle::run_particle_filter(
    int t,
    const std::unique_ptr<Data>& data,
    const std::unique_ptr<Distance>& distfun,
    const std::unique_ptr<PartitionFunction>& pfun
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
    
    particle_filters[0].log_weight(t) = log_likelihood_increment(t);
    log_weight(t) += log_likelihood_increment(t);
  } else {
    Rcpp::stop("Not implemented yet.");
  }
}
