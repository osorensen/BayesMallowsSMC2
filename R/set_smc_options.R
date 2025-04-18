#' Set SMC options
#'
#' @param n_particles Number of particles
#' @param n_particle_filters Initial number of particle filters for each
#'   particle
#' @param max_particle_filters Maximum number of particle filters.
#' @param resampling_threshold Effective sample size threshold for resampling
#' @param doubling_threshold Threshold for particle filter doubling. If the
#'   acceptance rate of the rejuvenation step falls below this threshold, the
#'   number of particle filters is doubled. Defaults to 0.2.
#' @param max_rejuvenation_steps Maximum number of rejuvenation steps. If the
#'   number of unique particles has not exceeded half the number of particles
#'   after this many steps, the rejuvenation is still stopped.
#' @param metric Metric
#' @param resampler resampler
#' @param latent_rank_proposal latent rank proposal
#' @param verbose Boolean
#' @param trace Logical specifying whether to save static parameters at each
#'   timestep.
#' @param trace_latent Logical specifying whether to sample and save one
#'   complete set of latent rankings for each particle and each timepoint.
#'
#' @return A list
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
