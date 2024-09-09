#' Set hyperparameters
#'
#' @param n_items Integer defining the number of items.
#' @param alpha_shape Shape of gamma prior for alpha.
#' @param alpha_rate Rate of gamma prior for alpha.
#' @param cluster_concentration Concentration parameter of Dirichlet distribution for cluster probabilities.
#' @param n_clusters Integer defining the number of clusters.
#'
#' @return A list
#' @export
#'
set_hyperparameters <- function(
    n_items, alpha_shape = 1, alpha_rate = .5, cluster_concentration = 10,
    n_clusters = 1) {
  if(missing(n_items)) stop("n_items must be provided")
  as.list(environment())
}
