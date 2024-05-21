#' Set SMC options
#'
#' @param n_particles Number of particles
#' @param n_particle_filters Initial number of particle filters for each particle
#' @param max_particle_filters Maximum number of particle filters.
#' @param resampling_threshold Effective sample size threshold for resampling
#' @param metric Metric
#' @param resampler resampler
#' @param verbose Boolean
#' @param trace Logical specifying whether to save static parameters at each
#' timestep.
#'
#' @return A list
#' @export
#'
set_smc_options <- function(
    n_particles = 1000, n_particle_filters = 50, max_particle_filters = 10000,
    resampling_threshold = n_particles / 2,
    metric = "footrule", resampler = "multinomial", verbose = FALSE, trace = FALSE) {
  as.list(environment())
}
