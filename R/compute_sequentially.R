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
#' If data contains pairwise preferences, it must have the following structure:
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
#'
#' @return An object
#' @export
#'
#' @examples
#' compute_sequentially(partial_rankings)
compute_sequentially <- function(
    data,
    hyperparameters = set_hyperparameters(),
    smc_options = set_smc_options()){
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
    input_timeseries <- split(data, f = ~ timepoint) |>
      lapply(split, f = ~ user) |>
      lapply(function(x) lapply(x, function(y) as.matrix(y[preference_columns])))
    attr(input_timeseries, "type") <- "pairwise preferences"
  } else {
    stop("Something wrong with data")
  }

  ret <- run_smc(input_timeseries, hyperparameters, smc_options)
}
