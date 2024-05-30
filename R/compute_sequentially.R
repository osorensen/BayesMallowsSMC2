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
#' \item etc.
#' }
#'
#' @param hyperparameters A list returned from [set_hyperparameters()].
#' @param smc_options A list returned from [set_smc_options()]
#' @param topological_sorts_directory Path to a directory where precomputed
#'   topological sorts can be found. Must contain subdirectories with names
#'   `user1`, `user2`, etc., exactly matching the `user` column of `data`. See
#'   [precompute_topological_sorts()]. Defaults to `NULL`, but has to be
#'   provided when data contain pairwise preferences.
#'
#' @param num_topological_sorts Integer vector containing the number of
#'   topological sorts for each user.
#'
#' @return An object
#' @export
#'
compute_sequentially <- function(
    data,
    hyperparameters = set_hyperparameters(),
    smc_options = set_smc_options(),
    topological_sorts_directory = NULL,
    num_topological_sorts = NULL
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
  } else if(sum(preference_columns) == 2) {
    if(is.null(topological_sorts_directory)) {
      stop("topological_sorts_directory must be provided with preference data.")
    }
    if(is.null(num_topological_sorts)) {
      stop("num_topological_sorts must be provided with preference data.")
    }
    input_timeseries <- split(data, f = ~ timepoint) |>
      lapply(split, f = ~ user) |>
      lapply(function(x) lapply(x, function(y) as.matrix(y[preference_columns])))
    attr(input_timeseries, "type") <- "pairwise preferences"
    attr(input_timeseries, "topological_sorts_directory") <- topological_sorts_directory
    names(num_topological_sorts) <- as.character(lapply(input_timeseries, names))
    attr(input_timeseries, "num_topological_sorts") <- num_topological_sorts
  } else {
    stop("Something wrong with data")
  }

  ret <- run_smc(input_timeseries, hyperparameters, smc_options)
}
