#' Simulated data with complete rankings
#'
#' A simulated dataset containing complete rankings of ten items by 400 users,
#' at 400 separate timepoints. The dataset was simulated using the 
#' `sample_mallows` function of the `BayesMallows` package 
#' \insertCite{sorensenBayesMallowsPackageBayesian2020}{BayesMallowsSMC2}.
#'
#' @format 
#' A data frame with 400 rows and 12 columns:
#' \describe{
#'   \item{timepoint}{Integer specifying the timepoint.}
#'   \item{user}{Integer specifying the user id.}
#'   \item{item1, item2,...}{Ranking given to each item.}
#' }
#' 
#' @references 
#' \insertAllCited{}
"complete_rankings"

#' Simulated data with top-3 rankings
#'
#' A simulated dataset containing top-3 rankings of ten items by 400 users,
#' at 400 separate timepoints. The dataset was simulated using the 
#' `sample_mallows` function of the `BayesMallows` package 
#' \insertCite{sorensenBayesMallowsPackageBayesian2020}{BayesMallowsSMC2}, and 
#' is identical to [complete_rankings] with rankings below 3 set to `NA`.
#'
#' @format 
#' A data frame with 400 rows and 12 columns:
#' \describe{
#'   \item{timepoint}{Integer specifying the timepoint.}
#'   \item{user}{Integer specifying the user id.}
#'   \item{item1, item2,...}{Ranking given to each item.}
#' }
#' 
#' @references 
#' \insertAllCited{}
"partial_rankings"

#' Simulated data with two clusters
#'
#' A simulated dataset containing complete rankings of ten items by 400 users,
#' at 400 separate timepoints. The dataset was simulated using the 
#' `sample_mallows` function of the `BayesMallows` package 
#' \insertCite{sorensenBayesMallowsPackageBayesian2020}{BayesMallowsSMC2}, and
#' the users were randomly assigned to one of two mixture components.
#'
#' @format 
#' A data frame with 400 rows and 12 columns:
#' \describe{
#'   \item{timepoint}{Integer specifying the timepoint.}
#'   \item{user}{Integer specifying the user id.}
#'   \item{item1, item2,...}{Ranking given to each item.}
#' }
#' 
#' @references 
#' \insertAllCited{}
"complete_cluster_rankings"