#' Print Method for BayesMallowsSMC2 Objects
#'
#' Prints a summary of a BayesMallowsSMC2 object returned by
#' [compute_sequentially()].
#'
#' @param x An object of class \code{BayesMallowsSMC2}.
#' @param ... Additional arguments (currently unused).
#'
#' @return Invisibly returns the input object \code{x}.
#'
#' @details
#' The print method displays key information about the fitted Bayesian Mallows
#' model, including:
#' \itemize{
#'   \item Number of particles
#'   \item Number of timepoints
#'   \item Number of items
#'   \item Number of clusters
#'   \item Log marginal likelihood
#'   \item Final effective sample size (ESS)
#'   \item Number of resampling events
#' }
#'
#' @export
#'
#' @examples
#' # Fit a model with complete rankings
#' set.seed(123)
#' mod <- compute_sequentially(
#'   complete_rankings,
#'   hyperparameters = set_hyperparameters(n_items = 5),
#'   smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
#' )
#'
#' # Print the model
#' print(mod)
#'
#' # or simply
#' mod
#'
print.BayesMallowsSMC2 <- function(x, ...) {
  # Basic validation
  if (!inherits(x, "BayesMallowsSMC2")) {
    stop("x must be an object of class 'BayesMallowsSMC2'")
  }
  
  required_fields <- c("alpha", "rho", "ESS", "resampling", "log_marginal_likelihood")
  missing_fields <- setdiff(required_fields, names(x))
  if (length(missing_fields) > 0) {
    stop("Object is missing required fields: ", paste(missing_fields, collapse = ", "))
  }
  
  # Extract dimensions
  n_particles <- ncol(x$alpha)
  n_timepoints <- length(x$ESS)
  n_items <- dim(x$rho)[1]
  n_clusters <- nrow(x$alpha)
  
  # Count resampling events
  n_resampling_events <- sum(x$resampling)
  
  # Create header
  cat("BayesMallowsSMC2 Model\n")
  cat(strrep("=", nchar("BayesMallowsSMC2 Model")), "\n\n", sep = "")
  
  # Display basic information
  cat(sprintf("%-25s %s\n", "Number of particles:", n_particles))
  cat(sprintf("%-25s %s\n", "Number of timepoints:", n_timepoints))
  cat(sprintf("%-25s %s\n", "Number of items:", n_items))
  cat(sprintf("%-25s %s\n\n", "Number of clusters:", n_clusters))
  
  # Display model fit information
  cat(sprintf("%-25s %.2f\n", "Log marginal likelihood:", x$log_marginal_likelihood))
  cat(sprintf("%-25s %.2f\n", "Final ESS:", x$ESS[n_timepoints]))
  cat(sprintf("%-25s %d/%d\n", "Resampling events:", n_resampling_events, n_timepoints))
  
  invisible(x)
}
