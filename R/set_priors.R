#' Set prior parameters for Bayesian Mallows model
#'
#' This function is used to set the parameters of the prior distributions for 
#' the static parameters. For the dispersion parameter `alpha` we use a gamma
#' prior, specified with `alpha_shape` and `alpha_rate`. For the Dirichlet
#' process mixture, the parameter is `cluster_concentration`. For the modal
#' ranking we always use a uniform prior, and this is hence not specified.
#'
#' @param alpha_shape Shape parameter of gamma prior for `alpha`.
#' @param alpha_rate Rate parameter of gamma prior for `alpha`.
#' @param cluster_concentration Concentration parameter of Dirichlet prior for
#' `tau`.
#' @param n_clusters Integer specifying the number of clusters.
#'
#' @return An object to be provided to the `priors` argument of
#' [compute_sequentially()].
#' @export
#'
set_priors <- function(
    alpha_shape = 1,
    alpha_rate = .5,
    cluster_concentration = 10,
    n_clusters = 1
) {
  as.list(environment())
}