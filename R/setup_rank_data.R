#' Setup rank data for sequential estimation
#' 
#' This function is used to prepare and check rank and preference datasets to
#' ensure they are ready for further processing by [compute_sequentially()]. 
#' With complete or partial rankings, the `rankings` argument must be provided.
#' With pairwise preference data, the `preferences` argument must be provided.
#' 
#' @param rankings A `data.frame` containing the following columns:
#' \describe{
#'   \item{timepoint}{Integer specifying the timepoint.}
#'   \item{user}{Integer specifying the user id.}
#'   \item{item1, item2,...}{Ranking given to each item.}
#' }
#' @param preferences A `data.frame` containing the following columns:
#' \describe{
#'   \item{timepoint}{Integer specifying the timepoint.}
#'   \item{user}{Integer specifying the user id.}
#'   \item{bottom_item}{Integer identifying the disfavored item.}
#'   \item{top_item}{Integer identifying the preferred item.}
#' }
#'
#' @details
#' The timepoints, users, and items must all be identified using integers 
#' starting at 1. 
#' 
#' @return An object to be provided to the `data` argument of 
#'   [compute_sequentially()].
#' @export
#'
setup_rank_data <- function(
    rankings,
    preferences
) {
  if(!xor(missing(rankings), missing(preferences))) {
    stop("Either rankings or preferences must be provided")
  }
  
  if(missing(preferences)) {
    type <- "partial_rankings"
    x <- rankings
    rm(rankings)
  } else {
    type <- "preferences"
    x <- preferences
    rm(preferences)
  }
  if(colnames(x)[[1]] != "timepoint") {
    stop("The first column must be named 'timepoint'")
  }
  if(colnames(x)[[2]] != "user") {
    stop("The second column must be named 'user'")
  }
  if(nrow(x) == 0 || ncol(x) <= 2) {
    stop("No data, nothing to do")
  }
  
  if(type == "partial_rankings") {
    n_items <- length(grep("^item[0-9]+$", colnames(x)))
    if(sum(is.na(x[, -(1:2)])) == 0) {
      type <- "complete_rankings"
    }
  } else {
    n_items <- max(x[, -(1:2)])
  }
  
  x <- as.matrix(x)
  mode(x) <- "integer"
  
  as.list(environment())
}