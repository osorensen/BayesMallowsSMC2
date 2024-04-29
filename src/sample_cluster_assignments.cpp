#include "sample_cluster_assignments.h"

using namespace arma;

uvec sample_cluster_assignments(
    const umat& latent_rankings,
    const StaticParameters& parameters,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun
) {
  int n_new_users = latent_rankings.n_cols;
  int n_clusters = parameters.alpha.size();
  mat log_cluster_probabilities(n_clusters, n_new_users);
  uvec cluster_assignment(n_new_users);

  for(size_t user{}; user < n_new_users; user++) {
    for(size_t cluster{}; cluster < n_clusters; cluster++) {
      log_cluster_probabilities(cluster, user) = log(parameters.tau(cluster)) -
        pfun->logz(parameters.alpha(cluster)) - parameters.alpha(cluster) *
        distfun->d(latent_rankings.col(user), parameters.rho.col(cluster));
    }
    double maxval = log_cluster_probabilities.col(user).max();
    log_cluster_probabilities.col(user) = log_cluster_probabilities.col(user) -
      (maxval + log(sum(exp(log_cluster_probabilities.col(user) - maxval))));

    Rcpp::IntegerVector tmpclus = Rcpp::sample(
      parameters.alpha.size(), 1, false,
      Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(exp(log_cluster_probabilities.col(user)))),
      false
    );
    cluster_assignment(user) = tmpclus(0);
  }

  return cluster_assignment;
}
