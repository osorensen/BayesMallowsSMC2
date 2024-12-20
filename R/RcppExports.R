# Generated by using Rcpp::compileAttributes() -> do not edit by hand
# Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#' Precompute All Topological Sorts
#'
#' This function precomputes all topological sorts for a given preference matrix and
#' saves them to a specified output directory. It ensures the output directory exists
#' and creates it if it does not.
#'
#' @param prefs An integer matrix (`arma::umat`) representing the preference relations.
#' @param n_items An integer specifying the number of items to sort.
#' @param output_directory A string specifying the directory where the output files will be saved.
#' @param save_frac Number between 0 and 1 specifying which fraction of sorts to save.
#'
#' @details
#' The function generates all possible topological sorts for the provided preference matrix
#' and saves each sort as a binary file in the specified output directory. The output files
#' are named sequentially as `sort0.bin`, `sort1.bin`, and so on.
#'
#' @return This function returns the number of topological sorts.
#'
#' @export
precompute_topological_sorts <- function(prefs, n_items, output_directory, save_frac) {
    .Call(`_BayesMallowsSMC2_precompute_topological_sorts`, prefs, n_items, output_directory, save_frac)
}

run_smc <- function(input_timeseries, input_prior, input_options) {
    .Call(`_BayesMallowsSMC2_run_smc`, input_timeseries, input_prior, input_options)
}

