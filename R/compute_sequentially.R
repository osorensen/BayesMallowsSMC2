#' Compute the Bayesian Mallows model sequentially
#'
#' @description
#' Fits the Bayesian Mallows model to sequentially arriving ranking or preference
#' data using the SMC2 (Sequential Monte Carlo Squared) algorithm. This function
#' can handle complete rankings, partial rankings, and pairwise preference data,
#' and supports mixture models with multiple clusters.
#'
#' @param data A dataframe containing partial rankings or pairwise preferences.
#'   If `data` contains complete or partial rankings, it must have the following
#'   columns:
#'
#' \itemize{
#' \item `timepoint`: a numeric vector denoting the timepoint, starting at 1.
#' \item `user`: a vector identifying the user.
#' \item `item1`: ranking of item 1.
#' \item `item2`: ranking of item 2.
#' \item etc.
#' }
#'
#'   If data contains pairwise preferences, it must have the following
#'   structure:
#'
#' \itemize{
#' \item `timepoint`: a numeric vector denoting the timepoint, starting at 1.
#' \item `user`: a vector identifying the user.
#' \item `top_item`: identifier for the preferred item.
#' \item `bottom_item`: identifier for the dispreferred item.
#' }
#'
#' @param hyperparameters A list returned from [set_hyperparameters()].
#' @param smc_options A list returned from [set_smc_options()]
#' @param topological_sorts A list returned from
#'   [precompute_topological_sorts()]. Only used with preference data, and
#'   defaults to `NULL`.
#'
#' @return An object of class `BayesMallowsSMC2`, which is a list containing:
#' \describe{
#'   \item{alpha}{A matrix of dispersion parameter values with dimensions
#'     `[n_clusters, n_particles]`. Each column represents one particle, and
#'     each row represents one cluster.}
#'   \item{rho}{A 3-dimensional array of latent ranking values with dimensions
#'     `[n_items, n_clusters, n_particles]`. Entry `[i, k, j]` gives the rank
#'     of item `i` in cluster `k` for particle `j`.}
#'   \item{tau}{A matrix of precision parameter values (for mixture models) with
#'     dimensions `[n_clusters, n_particles]`. Each column represents one particle.}
#'   \item{cluster_probabilities}{A 3-dimensional array of cluster assignment
#'     probabilities (for mixture models) with dimensions
#'     `[n_particles, n_users, n_clusters]`. Only present when `n_clusters > 1`.}
#'   \item{importance_weights}{A numeric vector of length `n_particles` containing
#'     the normalized importance weights for each particle.}
#'   \item{ESS}{A numeric vector of length `n_timepoints` containing the effective
#'     sample size at each timepoint.}
#'   \item{resampling}{An integer vector of length `n_timepoints` indicating whether
#'     resampling occurred at each timepoint (1 = yes, 0 = no).}
#'   \item{n_particle_filters}{An integer vector of length `n_timepoints` showing
#'     the number of particle filters used at each timepoint.}
#'   \item{log_marginal_likelihood}{A numeric value giving the estimated log
#'     marginal likelihood of the data.}
#'   \item{alpha_traces}{A list of parameter traces (only if `trace = TRUE` in
#'     [set_smc_options()]). Each element contains alpha values at one timepoint.}
#'   \item{tau_traces}{A list of parameter traces (only if `trace = TRUE` in
#'     [set_smc_options()]). Each element contains tau values at one timepoint.}
#'   \item{rho_traces}{A list of parameter traces (only if `trace = TRUE` in
#'     [set_smc_options()]). Each element contains rho values at one timepoint.}
#'   \item{log_importance_weights_traces}{A list of importance weight traces
#'     (only if `trace = TRUE` in [set_smc_options()]).}
#'   \item{latent_rankings_traces}{A list of latent ranking traces (only if
#'     `trace_latent = TRUE` in [set_smc_options()]).}
#' }
#'
#' @details
#' The function implements the SMC2 algorithm for sequential Bayesian inference
#' in the Mallows model. At each timepoint, it updates the particle approximation
#' to the posterior distribution as new ranking or preference data arrives. The
#' algorithm automatically performs resampling and rejuvenation steps when the
#' effective sample size drops below the specified threshold.
#'
#' The returned object has S3 methods for printing ([print.BayesMallowsSMC2]),
#' summarizing ([summary.BayesMallowsSMC2]), and plotting ([plot.BayesMallowsSMC2]).
#' For visualization of parameter evolution over time, see [trace_plot()].
#'
#' @references
#' \insertRef{10.1214/25-BA1564}{BayesMallowsSMC2}
#'
#' @export
#'
#' @examples
#' # Compute the model sequentially with complete rankings
#' mod <- compute_sequentially(
#'   complete_rankings,
#'   hyperparameters = set_hyperparameters(n_items = 5),
#'   smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
#' )
#'
#' # Print the model summary
#' mod
#'
#' # Plot posterior distribution of alpha
#' plot(mod, parameter = "alpha")
#'
compute_sequentially <- function(
    data,
    hyperparameters = set_hyperparameters(),
    smc_options = set_smc_options(),
    topological_sorts = NULL
    ){
  rank_columns <- grepl("item[0-9]+", colnames(data))
  preference_columns <- grepl("top\\_item|bottom\\_item", colnames(data))

  if(any(rank_columns)) {
    input_timeseries <- split(data, f = ~ timepoint) |>
      lapply(split, f = ~ user) |>
      lapply(function(x) lapply(x, function(y) as.numeric(y[rank_columns])))

    if(any(is.na(data[rank_columns]))) {
      attr(input_timeseries, "type") <- "partial rankings"
    } else {
      attr(input_timeseries, "type") <- "complete rankings"
    }
    sort_matrices <- sort_counts <- list()
  } else if(sum(preference_columns) == 2) {
    if(is.null(topological_sorts)) {
      stop("topological_sorts must be provided with preference data.")
    }
    input_timeseries <- split(data, f = ~ timepoint) |>
      lapply(split, f = ~ user) |>
      lapply(function(x) lapply(x, function(y) as.matrix(y[preference_columns])))
    attr(input_timeseries, "type") <- "pairwise preferences"

    sort_matrices <- lapply(topological_sorts, function(x) {
      lapply(x, function(y) y$sort_matrix)
    })

    sort_counts <- lapply(topological_sorts, function(x) {
      lapply(x, function(y) y$sort_count)
    })
  } else {
    stop("Something wrong with data")
  }

  if(max(table(data$user)) > 1 &&
     attr(input_timeseries, "type") != "pairwise preferences") {
    stop("Updated users not supported.")
  }

  ret <- run_smc(input_timeseries, hyperparameters, smc_options,
                 sort_matrices, sort_counts)

  class(ret) <- "BayesMallowsSMC2"
  ret
}

