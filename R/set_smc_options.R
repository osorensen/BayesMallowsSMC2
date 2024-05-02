#' Set SMC options
#'
#' @param n_particles Number of particles
#' @param n_particle_filters Number of particle filters for each particle
#' @param resampling_threshold Effective sample size threshold for resampling
#' @param metric Metric
#' @param resampler resampler
#'
#' @return A list
#' @export
#'
set_smc_options <- function(
    n_particles = 1000, n_particle_filters = 50, resampling_threshold = n_particles / 2,
    metric = "footrule", resampler = "multinomial", verbose = FALSE) {
  as.list(environment())
}
