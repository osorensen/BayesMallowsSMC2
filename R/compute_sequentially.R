#' Estimate the Bayesian Mallows model sequentially
#'
#' @param data An object returned from [setup_rank_data()].
#' @param smc_options An object returned from [set_smc_options()].
#' @param priors An object returned from [set_priors()].
#' @param model_options An object returned from [set_model_options()].
#'
#' @return An object of class `BayesMallowsSMC2`.
#' @export
#' 
#' @references 
#' \insertRef{chopinSMC2EfficientAlgorithm2013}{BayesMallowsSMC2}
#' 
#' \insertRef{fulopEfficientLearningSimulation2013}{BayesMallowsSMC2}
#'
compute_sequentially <- function(
    data = setup_rank_data(),
    smc_options = set_smc_options(),
    priors = set_priors(),
    model_options = set_model_options()
) {
  ret <- run_smc(data, smc_options, priors, model_options)
  class(ret) <- "BayesMallowsSMC2"
  ret
}