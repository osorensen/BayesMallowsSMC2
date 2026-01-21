#' Compute the Bayesian Mallows model sequentially
#'
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
#' @return An object of class BayesMallowsSMC2.
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

