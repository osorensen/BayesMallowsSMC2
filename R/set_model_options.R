#' Set options for Bayesian Mallows model
#' 
#' This function returns an object with user-defined options, and should be
#' provided to [compute_sequentially()].
#'
#' @param metric The metric used. One of "footrule", "spearman", "kendall",
#' "cayley", "ulam", and "hamming". Defaults to "footrule".
#' @param n_clusters Integer specifying the number of clusters.
#'
#' @return An object to be provided to [compute_sequentially()].
#' @export
#'
set_model_options <- function(
    metric = c("footrule", "spearman", "kendall", "cayley", "ulam", "hamming"),
    n_clusters = 1
) {
  metric <- match.arg(
    metric, 
    c("footrule", "spearman", "kendall", "cayley", "ulam", "hamming"))
  as.list(environment())
}