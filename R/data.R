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

#' Simulated Data
#'
#' @format ## `complete_rankings_heterogeneous`
#' A data frame with 200 rows and 7 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{item1, item2, item3, item4, item5}{Ranking given to the item.}
#' }
"complete_rankings_heterogeneous"


#' Simulated Data
#'
#' @format ## `complete_rankings_mixtures`
#' A data frame with 1000 rows and 7 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{item1, item2, item3, item4, item5}{Ranking given to the item.}
#' }
"complete_rankings_mixtures"

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

#' Simulated Data with Missing Values
#'
#' @format ## `partial_rankings_heterogeneous`
#' A data frame with 200 rows and 7 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{item1, item2, item3, item4, item5}{Ranking given to the item.}
#' }
"partial_rankings_heterogeneous"


#' Simulated Data with Missing Values
#'
#' @format ## `partial_rankings_mixtures`
#' A data frame with 1000 rows and 7 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{item1, item2, item3, item4, item5}{Ranking given to the item.}
#' }
"partial_rankings_mixtures"

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

#' @format ## `pairwise_preferences_heterogeneous`
#' A data frame with 800 rows and 4 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{top_item}{Item preferred in the comparison.}
#'   \item{bottom_item}{Item disfavored in the comparison.}
#' }
"pairwise_preferences_heterogeneous"

#' Simulated Data with Pairwise Preferences
#'
#' @format ## `pairwise_preferences_mixtures`
#' A data frame with 4000 rows and 4 columns:
#' \describe{
#'   \item{timepoint}{Timepoint}
#'   \item{user}{User id}
#'   \item{top_item}{Item preferred in the comparison.}
#'   \item{bottom_item}{Item disfavored in the comparison.}
#' }
"pairwise_preferences_mixtures"
