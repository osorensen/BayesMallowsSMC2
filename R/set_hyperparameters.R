#' Set hyperparameters for Bayesian Mallows model
#'
#' This function creates a list of hyperparameters for the Bayesian Mallows model
#' used in sequential Monte Carlo inference. The hyperparameters define the prior
#' distributions for the model parameters.
#'
#' @param n_items Integer defining the number of items to be ranked. Must be a
#'   positive integer.
#' @param alpha_shape Positive numeric value specifying the shape parameter of 
#'   the gamma prior distribution for the scale parameter alpha. Default is 1.
#' @param alpha_rate Positive numeric value specifying the rate parameter of 
#'   the gamma prior distribution for the scale parameter alpha. Default is 0.5.
#' @param cluster_concentration Positive numeric value specifying the 
#'   concentration parameter of the Dirichlet distribution for cluster 
#'   probabilities. Default is 10.
#' @param n_clusters Positive integer defining the number of clusters in the 
#'   mixture model. Default is 1.
#'
#' @return A list containing the hyperparameter values with elements:
#'   \item{n_items}{Number of items}
#'   \item{alpha_shape}{Shape parameter for alpha prior}
#'   \item{alpha_rate}{Rate parameter for alpha prior}
#'   \item{cluster_concentration}{Concentration parameter for cluster probabilities}
#'   \item{n_clusters}{Number of clusters}
#'
#' @examples
#' # Basic hyperparameters for 5 items
#' hyper <- set_hyperparameters(n_items = 5)
#' 
#' # Custom hyperparameters with multiple clusters
#' hyper <- set_hyperparameters(
#'   n_items = 10,
#'   alpha_shape = 2,
#'   alpha_rate = 1,
#'   cluster_concentration = 5,
#'   n_clusters = 3
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
