#' Set SMC options for sequential inference
#'
#' This function creates a list of options for the Sequential Monte Carlo (SMC²)
#' algorithm used in Bayesian inference for the Mallows model. These options
#' control the behavior of the particle filtering and resampling procedures.
#'
#' @param n_particles Positive integer specifying the number of particles to use
#'   in the SMC algorithm. Default is 1000.
#' @param n_particle_filters Positive integer specifying the initial number of 
#'   particle filters for each particle. Default is 50.
#' @param max_particle_filters Positive integer specifying the maximum number 
#'   of particle filters allowed. Default is 10000.
#' @param resampling_threshold Positive numeric value specifying the effective 
#'   sample size threshold for triggering resampling. Default is n_particles/2.
#' @param doubling_threshold Numeric value between 0 and 1 specifying the 
#'   threshold for particle filter doubling. If the acceptance rate of the 
#'   rejuvenation step falls below this threshold, the number of particle 
#'   filters is doubled. Default is 0.2.
#' @param max_rejuvenation_steps Positive integer specifying the maximum number 
#'   of rejuvenation steps. If the number of unique particles has not exceeded 
#'   half the number of particles after this many steps, the rejuvenation is 
#'   stopped. Default is 20.
#' @param metric Character string specifying the distance metric to use. 
#'   Options include "footrule", "kendall", "spearman", "cayley", "hamming", 
#'   and "ulam". Default is "footrule".
#' @param resampler Character string specifying the resampling method. 
#'   Options include "multinomial", "residual", "stratified", and "systematic". 
#'   Default is "multinomial".
#' @param latent_rank_proposal Character string specifying the proposal 
#'   distribution for latent rankings. Default is "uniform".
#' @param verbose Logical value indicating whether to print progress messages 
#'   during computation. Default is FALSE.
#' @param trace Logical value specifying whether to save static parameters at 
#'   each timestep. Default is FALSE.
#' @param trace_latent Logical value specifying whether to sample and save one 
#'   complete set of latent rankings for each particle and each timepoint. 
#'   Default is FALSE.
#'
#' @return A list containing all the SMC options with the specified values.
#'
#' @examples
#' # Default SMC options
#' opts <- set_smc_options()
#' 
#' # Custom SMC options with fewer particles and Kendall distance
#' opts <- set_smc_options(
#'   n_particles = 500,
#'   n_particle_filters = 25,
#'   metric = "kendall",
#'   verbose = TRUE
#' )
#' 
#' # Options for tracing parameters
#' opts <- set_smc_options(
#'   n_particles = 100,
#'   trace = TRUE,
#'   trace_latent = TRUE
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
