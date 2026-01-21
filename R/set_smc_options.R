#' Set SMC options
#'
#' @description
#' Configure the SMC2 (Sequential Monte Carlo Squared) algorithm for the
#' Bayesian Mallows model. This function sets parameters that control the
#' particle filter, resampling strategy, and diagnostic output.
#'
#' @param n_particles Integer specifying the number of particles to use in the
#'   outer SMC loop. More particles generally improve approximation accuracy but
#'   increase computational cost. Defaults to 1000.
#' @param n_particle_filters Integer specifying the initial number of particle
#'   filters for each particle in the inner loop. This controls the granularity
#'   of the latent rank estimation. Defaults to 50.
#' @param max_particle_filters Integer specifying the maximum number of particle
#'   filters allowed. The algorithm can adaptively increase the number of
#'   filters up to this limit when the acceptance rate is low. Defaults to
#'   10000.
#' @param resampling_threshold Numeric specifying the effective sample size
#'   threshold for triggering resampling. When the effective sample size falls
#'   below this threshold, the particles are resampled to avoid degeneracy.
#'   Defaults to `n_particles / 2`.
#' @param doubling_threshold Numeric threshold for particle filter doubling. If
#'   the acceptance rate of the rejuvenation step falls below this threshold,
#'   the number of particle filters is doubled (up to `max_particle_filters`)
#'   to improve mixing. Should be between 0 and 1. Defaults to 0.2.
#' @param max_rejuvenation_steps Integer specifying the maximum number of
#'   rejuvenation MCMC steps to perform. The rejuvenation step helps maintain
#'   particle diversity. The algorithm stops early if the number of unique
#'   particles exceeds half the total number of particles. Defaults to 20.
#' @param metric Character string specifying the distance metric to use for
#'   comparing rankings. Options are `"footrule"` (default), `"spearman"`,
#'   `"kendall"`, `"cayley"`, `"hamming"`, or `"ulam"`. The choice of metric
#'   affects the likelihood function in the Mallows model.
#' @param resampler Character string specifying the resampling algorithm.
#'   Options are `"multinomial"` (default), `"residual"`, `"stratified"`, or
#'   `"systematic"`. Different resamplers have different variance properties.
#' @param latent_rank_proposal Character string specifying the proposal
#'   distribution for latent ranks in the Metropolis-Hastings step. Options are
#'   `"uniform"` (default) or `"pseudo"`. The `"pseudo"` option can provide
#'   better proposals for partial rankings.
#' @param verbose Logical indicating whether to print progress messages during
#'   computation. Defaults to `FALSE`.
#' @param trace Logical specifying whether to save static parameters (alpha,
#'   rho, cluster probabilities) at each timestep. This is useful for
#'   diagnostic purposes but increases memory usage. Defaults to `FALSE`.
#' @param trace_latent Logical specifying whether to sample and save one
#'   complete set of latent rankings for each particle at each timepoint. This
#'   can be used to inspect the evolution of rankings over time but
#'   substantially increases memory usage. Defaults to `FALSE`.
#'
#' @details
#' The SMC2 algorithm uses a nested particle filter structure:
#' \itemize{
#'   \item The outer loop maintains `n_particles` particles, each representing
#'     a hypothesis about the static parameters (alpha, rho, cluster
#'     probabilities).
#'   \item The inner loop uses `n_particle_filters` particle filters per outer
#'     particle to estimate the latent rankings.
#'   \item When new data arrives, the algorithm updates the particle weights and
#'     performs rejuvenation MCMC steps to maintain diversity.
#'   \item If particle degeneracy occurs (effective sample size below
#'     `resampling_threshold`), particles are resampled.
#'   \item If the acceptance rate during rejuvenation is low (below
#'     `doubling_threshold`), the number of particle filters is adaptively
#'     doubled.
#' }
#'
#' For computational efficiency with CRAN examples, use smaller values such as
#' `n_particles = 100` and `n_particle_filters = 1`.
#'
#' @return A list containing all the specified options, suitable for passing to
#'   [compute_sequentially()].
#'
#' @seealso [compute_sequentially()], [set_hyperparameters()]
#'
#' @export
#'
#' @examples
#' # Basic usage with default settings
#' opts <- set_smc_options()
#'
#' # Customize for faster computation (suitable for CRAN examples)
#' opts_fast <- set_smc_options(
#'   n_particles = 100,
#'   n_particle_filters = 1
#' )
#'
#' # Use with complete rankings data
#' mod <- compute_sequentially(
#'   complete_rankings,
#'   hyperparameters = set_hyperparameters(n_items = 5),
#'   smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
#' )
#'
#' # Customize resampling and metric
#' opts_custom <- set_smc_options(
#'   n_particles = 200,
#'   n_particle_filters = 10,
#'   resampler = "stratified",
#'   metric = "kendall"
#' )
#'
#' # Enable diagnostic output
#' opts_trace <- set_smc_options(
#'   n_particles = 100,
#'   n_particle_filters = 1,
#'   verbose = TRUE,
#'   trace = TRUE
#' )
#'
#' # For partial rankings with more rejuvenation steps
#' mod_partial <- compute_sequentially(
#'   partial_rankings[1:10, ],
#'   hyperparameters = set_hyperparameters(n_items = 5),
#'   smc_options = set_smc_options(
#'     n_particles = 30,
#'     n_particle_filters = 5,
#'     max_rejuvenation_steps = 10,
#'     latent_rank_proposal = "uniform"
#'   )
#' )
#'
set_smc_options <- function(
    n_particles = 1000, n_particle_filters = 50, max_particle_filters = 10000,
    resampling_threshold = n_particles / 2, doubling_threshold = .2,
    max_rejuvenation_steps = 20,
    metric = "footrule", resampler = "multinomial",
    latent_rank_proposal = "uniform", verbose = FALSE,
    trace = FALSE, trace_latent = FALSE) {
  as.list(environment())
}
