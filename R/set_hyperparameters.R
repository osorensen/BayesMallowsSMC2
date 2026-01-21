#' Set hyperparameters
#'
#' Set the hyperparameters for the Bayesian Mallows model. This function
#' creates a list of hyperparameter values that can be passed to
#' [compute_sequentially()].
#'
#' @param n_items Integer defining the number of items.
#' @param alpha_shape Shape parameter of the gamma prior distribution for the
#'   scale parameter alpha. Defaults to 1.
#' @param alpha_rate Rate parameter of the gamma prior distribution for the
#'   scale parameter alpha. Defaults to 0.5.
#' @param cluster_concentration Concentration parameter of the Dirichlet
#'   distribution for cluster probabilities. Only used when `n_clusters > 1`.
#'   Defaults to 10.
#' @param n_clusters Integer defining the number of clusters. Defaults to 1.
#'
#' @return A list with components `n_items`, `alpha_shape`, `alpha_rate`,
#'   `cluster_concentration`, and `n_clusters`.
#' @export
#'
#' @examples
#' # Example: Set hyperparameters and use them with partial rankings
#' # Set hyperparameters with default values
#' hyperparams1 <- set_hyperparameters(n_items = 5)
#' 
#' # Set hyperparameters with custom prior for alpha
#' # A larger alpha_shape and smaller alpha_rate increases the prior mean
#' hyperparams2 <- set_hyperparameters(
#'   n_items = 5, 
#'   alpha_shape = 2, 
#'   alpha_rate = 1
#' )
#' 
#' # Use the hyperparameters with compute_sequentially
#' # This example uses partial rankings with a small number of particles
#' # for fast execution suitable for CRAN checks
#' set.seed(123)
#' mod <- compute_sequentially(
#'   partial_rankings,
#'   hyperparameters = hyperparams2,
#'   smc_options = set_smc_options(
#'     n_particles = 20, 
#'     n_particle_filters = 4,
#'     max_rejuvenation_steps = 3
#'   )
#' )
#'
set_hyperparameters <- function(
    n_items, alpha_shape = 1, alpha_rate = .5, cluster_concentration = 10,
    n_clusters = 1) {
  if(missing(n_items)) stop("n_items must be provided")
  as.list(environment())
}
