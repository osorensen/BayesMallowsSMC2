#' Set options for SMC2 algorithm
#' 
#' This function is used to set the options for the SMC2 algorithm used to 
#' estimate the Bayesian Mallows model.
#' 
#' @param n_particles Integer specifying the number of particles used in the
#' top-level iterated batch importance sampling algorithm.
#' @param n_particle_filters Integer specifying the number of particle filters
#' to run for each top-level particle.
#' @param resampler Resampler to use. One of "multinomial", "residual", 
#' "stratified", or "systematic". 
#' @param max_rejuvenation_steps Maximum number of MCMC steps in rejuvenation 
#' algorithm.
#' @param verbose Whether to report progress or not.
#' 
#' @return An object to be provided to the `smc_options` argument of 
#' [compute_sequentially()].
#' @export
#'
set_smc_options <- function(
    n_particles,
    n_particle_filters,
    resampler = c("multinomial", "residual", "stratified", "systematic"),
    max_rejuvenation_steps = 10,
    verbose = FALSE
    ) {
  if(missing(n_particles) || missing(n_particle_filters)) {
    stop("Both n_particles and n_particle_filters must be provided.")
  }
  if(n_particles < 1) {
    stop("n_particles must be a positive integer.")
  }
  resampler <- match.arg(resampler, c("multinomial", "residual", "stratified",
                                      "systematic"))
  
  as.list(environment())
}