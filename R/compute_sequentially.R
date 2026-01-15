#' Compute the Bayesian Mallows model sequentially
#'
#' This function implements the nested sequential Monte Carlo (SMC2) algorithm
#' for sequential learning of rank and preference data using the Bayesian
#' Mallows model. The algorithm processes data sequentially over time,
#' maintaining a particle approximation to the posterior distribution.
#'
#' @details
#' The nested SMC2 algorithm consists of two levels of sequential Monte Carlo:
#' 
#' **Outer SMC (Parameter Level)**: Maintains particles representing samples
#' from the posterior distribution of static parameters (alpha, rho, tau).
#' Each particle contains its own set of parameter values.
#' 
#' **Inner SMC (Latent State Level)**: For each parameter particle, maintains
#' multiple particle filters that track the evolution of latent rankings and
#' cluster assignments over time. This nested structure allows the algorithm
#' to handle the complex dependency structure between parameters and latent
#' states.
#' 
#' At each timepoint, the algorithm:
#' 1. Propagates each parameter particle forward using MCMC moves
#' 2. For each parameter particle, runs multiple particle filters to sample
#'    latent rankings and cluster assignments
#' 3. Computes importance weights based on the likelihood of observed data
#' 4. Resamples particles when effective sample size drops below threshold
#' 5. Applies rejuvenation moves to maintain particle diversity
#' 
#' The nested structure is essential for maintaining proper uncertainty
#' quantification in the joint parameter-latent state space.
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
#'   If data contains pairwise preferences, it must have the following
#'   structure:
#'
#' \itemize{
#' \item `timepoint`: a numeric vector denoting the timepoint, starting at 1.
#' \item `user`: a vector identifying the user.
#' \item `top_item`: identifier for the preferred item.
#' \item `bottom_item`: identifier for the dispreferred item.
#' }
#'
#' @param hyperparameters A list returned from [set_hyperparameters()].
#' @param smc_options A list returned from [set_smc_options()]. Controls the
#'   nested SMC2 algorithm parameters including number of parameter particles,
#'   number of particle filters per parameter particle, and MCMC move parameters.
#' @param topological_sorts A list returned from
#'   [precompute_topological_sorts()]. Only used with preference data, and
#'   defaults to `NULL`. Contains precomputed topological sorts for efficient
#'   sampling from constrained ranking spaces.
#'
#' @return An object of class BayesMallowsSMC2 containing posterior samples
#'   and algorithm diagnostics.
#'
#' @references
#' Sørensen, Ø., Stein, A., Netto, W. L., & Leslie, D. S. (2025). 
#' Sequential Rank and Preference Learning with the Bayesian Mallows Model. 
#' \emph{Bayesian Analysis}. DOI: 10.1214/25-BA1564.
#'
#' @seealso [set_hyperparameters()], [set_smc_options()], [precompute_topological_sorts()]
#'
#' @examples
#' # Example with complete rankings
#' library(BayesMallowsSMC2)
#' 
#' # Generate synthetic ranking data
#' set.seed(123)
#' n_items <- 5
#' n_users <- 20
#' n_timepoints <- 10
#' 
#' # Create synthetic data
#' data <- expand.grid(
#'   timepoint = 1:n_timepoints,
#'   user = 1:n_users
#' )
#' 
#' # Add random rankings for each item
#' for(i in 1:n_items) {
#'   data[[paste0("item", i)]] <- sample(1:n_items, nrow(data), replace = TRUE)
#' }
#' 
#' # Set hyperparameters
#' hyperparams <- set_hyperparameters(
#'   n_items = n_items,
#'   alpha_shape = 2,
#'   alpha_rate = 1,
#'   n_clusters = 2
#' )
#' 
#' # Set SMC options
#' smc_opts <- set_smc_options(
#'   n_particles = 100,
#'   n_particle_filters = 20,
#'   metric = "kendall",
#'   verbose = TRUE
#' )
#' 
#' # Run sequential computation
#' result <- compute_sequentially(
#'   data = data,
#'   hyperparameters = hyperparams,
#'   smc_options = smc_opts
#' )
#' 
#' # Example with pairwise preferences
#' # First precompute topological sorts
#' prefs_matrix <- matrix(c(1, 2, 2, 3, 3, 1), ncol = 2, byrow = TRUE)
#' topo_sorts <- precompute_topological_sorts(
#'   prefs = prefs_matrix,
#'   n_items = 3,
#'   save_frac = 0.1
#' )
#' 
#' # Create preference data
#' pref_data <- data.frame(
#'   timepoint = c(1, 1, 2, 2),
#'   user = c(1, 2, 1, 2),
#'   top_item = c(1, 2, 3, 1),
#'   bottom_item = c(2, 3, 1, 3)
#' )
#' 
#' # Run with preferences
#' result_prefs <- compute_sequentially(
#'   data = pref_data,
#'   hyperparameters = set_hyperparameters(n_items = 3),
#'   smc_options = set_smc_options(n_particles = 50),
#'   topological_sorts = topo_sorts
#' )
#'
#' @export
#'
compute_sequentially <- function(
    data,
    hyperparameters = set_hyperparameters(),
    smc_options = set_smc_options(),
    topological_sorts = NULL
    ){
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
    sort_matrices <- sort_counts <- list()
  } else if(sum(preference_columns) == 2) {
    if(is.null(topological_sorts)) {
      stop("topological_sorts must be provided with preference data.")
    }
    input_timeseries <- split(data, f = ~ timepoint) |>
      lapply(split, f = ~ user) |>
      lapply(function(x) lapply(x, function(y) as.matrix(y[preference_columns])))
    attr(input_timeseries, "type") <- "pairwise preferences"

    sort_matrices <- lapply(topological_sorts, function(x) {
      lapply(x, function(y) y$sort_matrix)
    })

    sort_counts <- lapply(topological_sorts, function(x) {
      lapply(x, function(y) y$sort_count)
    })
  } else {
    stop("Something wrong with data")
  }

  if(max(table(data$user)) > 1 &&
     attr(input_timeseries, "type") != "pairwise preferences") {
    stop("Updated users not supported.")
  }

  ret <- run_smc(input_timeseries, hyperparameters, smc_options,
                 sort_matrices, sort_counts)

  class(ret) <- "BayesMallowsSMC2"
  ret
}

