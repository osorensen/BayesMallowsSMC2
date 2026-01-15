#' Set hyperparameters for Bayesian Mallows model
#'
#' Configure prior distributions and model structure for the Bayesian Mallows
#' model used in sequential estimation. This function sets hyperparameters
#' for the precision parameters, modal rankings, and cluster structure.
#'
#' @details
#' The Bayesian Mallows model assumes:
#' 
#' **Precision Parameters**: Each cluster k has a precision parameter alpha_k
#' that controls the concentration around the modal ranking. Higher values
#' indicate stronger agreement. The prior is Gamma(alpha_shape, alpha_rate).
#' 
#' **Modal Rankings**: Each cluster has a modal ranking rho_k that represents
#' the "consensus" ranking for that cluster. These are sampled uniformly
#' from the space of permutations.
#' 
#' **Cluster Probabilities**: The probability of assignment to each cluster
#' follows a Dirichlet distribution with concentration parameter
#' cluster_concentration.
#' 
#' **Cluster Structure**: The number of clusters must be specified a priori.
#' Model selection can be performed by comparing models with different
#' numbers of clusters.
#'
#' @param n_items Integer. Number of items being ranked. Must be provided
#'   and determines the dimensionality of the ranking space.
#' @param alpha_shape Numeric. Shape parameter of the Gamma prior distribution
#'   for precision parameters alpha_k. Higher values concentrate the prior
#'   around higher precision values. Default is 1 (exponential prior).
#' @param alpha_rate Numeric. Rate parameter of the Gamma prior distribution
#'   for precision parameters alpha_k. Higher values favor lower precision
#'   (more dispersed rankings). Default is 0.5.
#' @param cluster_concentration Numeric. Concentration parameter of the
#'   Dirichlet prior for cluster assignment probabilities. Higher values
#'   favor more equal cluster sizes, while lower values allow more unbalanced
#'   clusters. Default is 10.
#' @param n_clusters Integer. Number of mixture components (clusters) in the
#'   model. Each cluster has its own modal ranking and precision parameter.
#'   Default is 1 (single-cluster model).
#'
#' @return A list containing all hyperparameter values for use in
#'   [compute_sequentially()].
#'
#' @references
#' Sørensen, Ø., Stein, A., Netto, W. L., & Leslie, D. S. (2025). 
#' Sequential Rank and Preference Learning with the Bayesian Mallows Model. 
#' \emph{Bayesian Analysis}. DOI: 10.1214/25-BA1564.
#'
#' @seealso [compute_sequentially()], [set_smc_options()]
#'
#' @examples
#' # Basic hyperparameters for 5 items, single cluster
#' basic_hyper <- set_hyperparameters(n_items = 5)
#' 
#' # Multiple clusters with informative priors
#' multi_cluster <- set_hyperparameters(
#'   n_items = 10,
#'   alpha_shape = 2,      # More concentrated precision prior
#'   alpha_rate = 1,       # Moderate precision values
#'   n_clusters = 3,       # Three mixture components
#'   cluster_concentration = 5  # Allow unbalanced clusters
#' )
#' 
#' # High-precision scenario (strong agreement expected)
#' high_precision <- set_hyperparameters(
#'   n_items = 8,
#'   alpha_shape = 5,      # Strong prior for high precision
#'   alpha_rate = 0.5,     # Favor high alpha values
#'   n_clusters = 2
#' )
#' 
#' # Low-precision scenario (weak agreement expected)
#' low_precision <- set_hyperparameters(
#'   n_items = 6,
#'   alpha_shape = 1,      # Weak prior
#'   alpha_rate = 2,       # Favor low alpha values
#'   n_clusters = 1
#' )
#'
#' @export
#'
set_hyperparameters <- function(
    n_items, alpha_shape = 1, alpha_rate = .5, cluster_concentration = 10,
    n_clusters = 1) {
  if(missing(n_items)) stop("n_items must be provided")
  as.list(environment())
}
