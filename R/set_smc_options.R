#' Set SMC options for nested sequential Monte Carlo algorithm
#'
#' Configure parameters for the nested SMC2 algorithm used in sequential
#' Bayesian Mallows model estimation. This function sets both outer-level
#' (parameter particle) and inner-level (latent state particle filter)
#' algorithm parameters.
#'
#' @details
#' The nested SMC2 algorithm requires careful tuning of both levels:
#' 
#' **Outer SMC Level**: Controls the number of parameter particles and their
#' rejuvenation through MCMC moves. More particles provide better approximation
#' but increase computational cost.
#' 
#' **Inner SMC Level**: Each parameter particle maintains multiple particle
#' filters for latent rankings. More filters improve latent state estimation
#' but multiply computational cost by the number of parameter particles.
#'
#' @param n_particles Integer. Number of parameter particles in the outer SMC
#'   sampler. Each particle represents a sample from the posterior distribution
#'   of static parameters (alpha, rho, tau). Larger values improve posterior
#'   approximation accuracy but increase computational cost linearly.
#' @param n_particle_filters Integer. Initial number of particle filters
#'   maintained by each parameter particle for sampling latent rankings and
#'   cluster assignments. This creates the nested structure where each of the
#'   `n_particles` parameter particles runs `n_particle_filters` inner particle
#'   filters.
#' @param max_particle_filters Integer. Maximum number of particle filters
#'   allowed per parameter particle. The algorithm may double the number of
#'   filters when rejuvenation acceptance rates are low, up to this limit.
#' @param resampling_threshold Numeric. Effective sample size threshold for
#'   triggering resampling of parameter particles. When ESS falls below this
#'   value, multinomial resampling is performed. Default is `n_particles / 2`.
#' @param doubling_threshold Numeric. Acceptance rate threshold for particle
#'   filter doubling during rejuvenation. If the acceptance rate of MCMC
#'   rejuvenation moves falls below this threshold, the number of particle
#'   filters is doubled to improve mixing. Defaults to 0.2.
#' @param max_rejuvenation_steps Integer. Maximum number of MCMC rejuvenation
#'   steps applied to parameter particles. Rejuvenation stops early if the
#'   number of unique particles exceeds half the total number of particles,
#'   indicating sufficient diversity.
#' @param metric Character. Distance metric for the Mallows model. Options
#'   include "kendall", "cayley", "hamming", "footrule", "spearman", and "ulam".
#'   Different metrics capture different aspects of ranking disagreement.
#' @param resampler Character. Resampling algorithm for parameter particles.
#'   Options include "multinomial", "residual", "stratified", and "systematic".
#'   Systematic resampling often provides better performance.
#' @param latent_rank_proposal Character. Proposal mechanism for sampling
#'   latent rankings in the inner particle filters. Options include "uniform"
#'   and other problem-specific proposals.
#' @param verbose Logical. Whether to print algorithm progress and diagnostics
#'   during execution. Useful for monitoring convergence and performance.
#' @param trace Logical. Whether to save static parameter values (alpha, rho,
#'   tau) from all particles at each timestep. Enables detailed posterior
#'   analysis but increases memory usage.
#' @param trace_latent Logical. Whether to sample and save one complete set
#'   of latent rankings for each parameter particle at each timepoint. Provides
#'   full posterior samples of latent states but significantly increases memory
#'   requirements.
#'
#' @return A list containing all SMC2 algorithm parameters for use in
#'   [compute_sequentially()].
#'
#' @references
#' Sørensen, Ø., Stein, A., Netto, W. L., & Leslie, D. S. (2025). 
#' Sequential Rank and Preference Learning with the Bayesian Mallows Model. 
#' \emph{Bayesian Analysis}. DOI: 10.1214/25-BA1564.
#'
#' @seealso [compute_sequentially()], [set_hyperparameters()]
#'
#' @examples
#' # Basic SMC options for small problems
#' basic_opts <- set_smc_options(
#'   n_particles = 100,
#'   n_particle_filters = 20,
#'   metric = "kendall"
#' )
#' 
#' # High-precision options for larger problems
#' precise_opts <- set_smc_options(
#'   n_particles = 1000,
#'   n_particle_filters = 100,
#'   max_particle_filters = 500,
#'   resampling_threshold = 500,
#'   metric = "footrule",
#'   resampler = "systematic",
#'   verbose = TRUE,
#'   trace = TRUE
#' )
#' 
#' # Memory-efficient options
#' efficient_opts <- set_smc_options(
#'   n_particles = 200,
#'   n_particle_filters = 30,
#'   trace = FALSE,
#'   trace_latent = FALSE,
#'   verbose = FALSE
#' )
#'
#' @export
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
