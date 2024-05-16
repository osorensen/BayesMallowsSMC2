#' Simulated Data
#'
#' @format ## `complete_rankings`
#' A data frame with 100 rows and 7 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{item1, item2, item3, item4, item5}{Ranking given to the item.}
#' }
"complete_rankings"

#' Simulated Data with Missing Values
#'
#' @format ## `partial_rankings`
#' A data frame with 100 rows and 7 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{item1, item2, item3, item4, item5}{Ranking given to the item.}
#' }
"partial_rankings"

#' Simulated Data with Pairwise Preferences
#'
#' @format ## `pairwise_preferences`
#' A data frame with 400 rows and 4 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{top_item}{Item preferred in the comparison.}
#'   \item{bottom_item}{Item disfavored in the comparison.}
#' }
"pairwise_preferences"

#' Simulated Two-Component Mixtures Data
#'
#' @format ## `mixtures`
#' A data frame with 400 rows and 7 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{item1, item2, item3, item4, item5}{Ranking given to the item.}
#' }
"mixtures"
