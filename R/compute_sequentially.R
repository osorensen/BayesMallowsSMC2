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
#'
#' @return An object
#' @export
#'
#' @examples
#' compute_sequentially(partial_rankings)
compute_sequentially <- function(data = NULL){
  if(any(grepl("item[0-9]+", colnames(data)))) {
    type <- "rankings"
  } else if(any(grepl("bottom\\_item", colnames(data))) &&
            any(grepl("top\\_item", colnames(data)))) {
    type <- "pairwise"
  } else {
    stop("Something wrong with data")
  }
  timeseries <- split(data, f = data$timepoint)
  run_smc(timeseries)
}
