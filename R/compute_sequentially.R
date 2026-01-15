#' Compute the Bayesian Mallows model sequentially
#'
#' This function performs sequential Bayesian inference for the Mallows model
#' using the SMC² (Sequential Monte Carlo squared) algorithm. It can handle
#' both complete/partial rankings and pairwise preference data that arrive
#' sequentially over time.
#'
#' @param data A data frame containing ranking or preference data with temporal
#'   structure. The data frame must include `timepoint` and `user` columns.
#'   
#'   For complete or partial rankings, additional columns should be:
#'   \itemize{
#'   \item `timepoint`: Numeric vector denoting the timepoint, starting at 1.
#'   \item `user`: Vector identifying the user providing the ranking.
#'   \item `item1`, `item2`, etc.: Rankings of items (use NA for missing items
#'     in partial rankings).
#'   }
#'   
#'   For pairwise preferences, the structure should be:
#'   \itemize{
#'   \item `timepoint`: Numeric vector denoting the timepoint, starting at 1.
#'   \item `user`: Vector identifying the user providing the preference.
#'   \item `top_item`: Identifier for the preferred item.
#'   \item `bottom_item`: Identifier for the less preferred item.
#'   }
#'
#' @param hyperparameters A list of hyperparameters returned from 
#'   \code{\link{set_hyperparameters}}. Defines the prior distributions for
#'   model parameters.
#' @param smc_options A list of SMC algorithm options returned from 
#'   \code{\link{set_smc_options}}. Controls the behavior of the particle
#'   filtering algorithm.
#' @param topological_sorts A list returned from 
#'   \code{\link{precompute_topological_sorts}}. Required when using pairwise
#'   preference data, otherwise should be \code{NULL} (default).
#'
#' @return An object of class \code{BayesMallowsSMC2} containing the results
#'   of the sequential inference, including parameter traces, log marginal
#'   likelihood estimates, and other algorithm diagnostics.
#'
#' @examples
#' # Example with complete rankings
#' set.seed(123)
#' n_items <- 4
#' 
#' # Create synthetic ranking data
#' ranking_data <- data.frame(
#'   timepoint = c(1, 1, 2, 2),
#'   user = c(1, 2, 3, 4),
#'   item1 = c(1, 2, 1, 3),
#'   item2 = c(2, 1, 3, 1),
#'   item3 = c(3, 4, 2, 4),
#'   item4 = c(4, 3, 4, 2)
#' )
#' 
#' # Set up hyperparameters and options
#' hyper <- set_hyperparameters(n_items = n_items)
#' opts <- set_smc_options(n_particles = 100, verbose = FALSE)
#' 
#' \dontrun{
#' # Run sequential inference (requires compiled C++ code)
#' result <- compute_sequentially(
#'   data = ranking_data,
#'   hyperparameters = hyper,
#'   smc_options = opts
#' )
#' }
#' 
#' @references
#' \insertRef{sorensen2025sequential}{BayesMallowsSMC2}
#' 
#' @export
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

  # Check that each user appears at most once across all timepoints
  user_timepoint_combinations <- unique(data[c("user", "timepoint")])
  if(max(table(user_timepoint_combinations$user)) > 1) {
    stop("Each user can only enter the pool once. Users appearing at multiple timepoints: ",
         paste(names(table(user_timepoint_combinations$user))[table(user_timepoint_combinations$user) > 1], 
               collapse = ", "))
  }

  ret <- run_smc(input_timeseries, hyperparameters, smc_options,
                 sort_matrices, sort_counts)

  class(ret) <- "BayesMallowsSMC2"
  ret
}

