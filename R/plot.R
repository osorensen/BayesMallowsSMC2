#' Plot Posterior Distributions for BayesMallowsSMC2 Objects
#'
#' Visualize posterior distributions of the parameters of the Bayesian Mallows
#' model after the final timepoint.
#'
#' @param x An object of class `BayesMallowsSMC2`, returned from
#'   [compute_sequentially()].
#' @param parameter Character string defining the parameter to plot. Available
#'   options are `"alpha"` (default), `"rho"`, and `"tau"`.
#' @param items The items to study in the plot for `rho`. Either a vector of
#'   item names or a vector of indices (1 to n_items). If `NULL` and there are
#'   more than 5 items, 5 items are selected randomly. Only used when
#'   `parameter = "rho"`.
#' @param ... Other arguments (currently unused).
#'
#' @return A ggplot object showing:
#' \itemize{
#'   \item For `parameter = "alpha"`: A histogram of the posterior distribution
#'     of the dispersion parameter.
#'   \item For `parameter = "tau"`: A histogram of the posterior distribution
#'     of the precision parameter.
#'   \item For `parameter = "rho"`: Bar charts showing the posterior probability
#'     of each ranking for each item separately.
#' }
#'
#' @details
#' The function uses importance weights from the SMC algorithm to compute
#' weighted posterior distributions. For mixture models (multiple clusters),
#' separate plots are created for each cluster using faceting.
#'
#' For `parameter = "rho"`, if the number of items is large (> 5) and `items`
#' is not specified, a random subset of 5 items is selected for visualization.
#' The random selection respects the current random seed, so results are
#' reproducible if you set a seed before calling the function (e.g., with
#' `set.seed()`).
#'
#' @export
#'
#' @examples
#' \donttest{
#' # Fit a model with complete rankings
#' mod <- compute_sequentially(
#'   complete_rankings,
#'   hyperparameters = set_hyperparameters(n_items = 5),
#'   smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
#' )
#'
#' # Plot alpha (default)
#' plot(mod)
#'
#' # Plot tau
#' plot(mod, parameter = "tau")
#'
#' # Plot rho for specific items
#' plot(mod, parameter = "rho", items = c(1, 3, 5))
#'
#' # Plot rho for all items (if <= 5 items)
#' plot(mod, parameter = "rho")
#' }
plot.BayesMallowsSMC2 <- function(x, parameter = "alpha", items = NULL, ...) {
  # Validate parameter
  parameter <- match.arg(parameter, c("alpha", "rho", "tau"))
  
  # Basic validation
  if (!inherits(x, "BayesMallowsSMC2")) {
    stop("x must be an object of class 'BayesMallowsSMC2'")
  }
  
  # Check for required fields
  required_fields <- c("alpha", "rho", "tau", "importance_weights")
  missing_fields <- setdiff(required_fields, names(x))
  if (length(missing_fields) > 0) {
    stop("Object is missing required fields: ", paste(missing_fields, collapse = ", "))
  }
  
  if (parameter == "alpha") {
    plot_alpha_smc(x)
  } else if (parameter == "tau") {
    plot_tau_smc(x)
  } else if (parameter == "rho") {
    plot_rho_smc(x, items)
  }
}

# Avoid R CMD check NOTE about undefined global variables used in ggplot2::aes()
utils::globalVariables(c("value", "probability"))


# Helper function to create weighted samples from parameter values
# @param values Numeric vector of parameter values
# @param weights Numeric vector of importance weights
# @param n_samples Number of samples to draw (default 10000)
# @return Numeric vector of weighted samples
create_weighted_samples <- function(values, weights, n_samples = 10000) {
  sample_probs <- weights / sum(weights)
  sampled_indices <- sample(seq_along(values), size = n_samples, 
                            replace = TRUE, prob = sample_probs)
  values[sampled_indices]
}


# Internal function to plot alpha parameter
plot_alpha_smc <- function(x) {
  # Extract alpha values and weights
  # alpha is a matrix: [n_clusters, n_particles]
  alpha_matrix <- x$alpha
  weights <- x$importance_weights
  
  # Get dimensions
  n_clusters <- nrow(alpha_matrix)
  n_particles <- ncol(alpha_matrix)
  
  # Create a data frame for plotting
  # Repeat each alpha value according to its weight to create weighted histogram
  plot_data_list <- vector("list", n_clusters)
  
  for (cluster in seq_len(n_clusters)) {
    alpha_vals <- alpha_matrix[cluster, ]
    
    # Create weighted samples by replicating values
    sampled_alpha <- create_weighted_samples(alpha_vals, weights)
    
    plot_data_list[[cluster]] <- data.frame(
      value = sampled_alpha,
      cluster = if (n_clusters > 1) paste0("Cluster ", cluster) else "All"
    )
  }
  
  plot_data <- do.call(rbind, plot_data_list)
  
  # Create histogram
  p <- ggplot2::ggplot(plot_data, ggplot2::aes(x = value)) +
    ggplot2::geom_histogram(bins = 30, fill = "steelblue", color = "white") +
    ggplot2::xlab(expression(alpha)) +
    ggplot2::ylab("Frequency") +
    ggplot2::theme_minimal()
  
  # Add faceting if multiple clusters
  if (n_clusters > 1) {
    p <- p + ggplot2::facet_wrap(~ cluster, scales = "free_x")
  }
  
  p
}


# Internal function to plot tau parameter
plot_tau_smc <- function(x) {
  # Extract tau values and weights
  # tau is a matrix: [n_clusters, n_particles]
  tau_matrix <- x$tau
  weights <- x$importance_weights
  
  # Get dimensions
  n_clusters <- nrow(tau_matrix)
  n_particles <- ncol(tau_matrix)
  
  # Create a data frame for plotting
  plot_data_list <- vector("list", n_clusters)
  
  for (cluster in seq_len(n_clusters)) {
    tau_vals <- tau_matrix[cluster, ]
    
    # Create weighted samples
    sampled_tau <- create_weighted_samples(tau_vals, weights)
    
    plot_data_list[[cluster]] <- data.frame(
      value = sampled_tau,
      cluster = if (n_clusters > 1) paste0("Cluster ", cluster) else "All"
    )
  }
  
  plot_data <- do.call(rbind, plot_data_list)
  
  # Create histogram
  p <- ggplot2::ggplot(plot_data, ggplot2::aes(x = value)) +
    ggplot2::geom_histogram(bins = 30, fill = "steelblue", color = "white") +
    ggplot2::xlab(expression(tau)) +
    ggplot2::ylab("Frequency") +
    ggplot2::theme_minimal()
  
  # Add faceting if multiple clusters
  if (n_clusters > 1) {
    p <- p + ggplot2::facet_wrap(~ cluster, scales = "free_x")
  }
  
  p
}


# Internal function to plot rho parameter
plot_rho_smc <- function(x, items = NULL) {
  # Extract rho array and weights
  # rho is a 3D array: [n_items, n_clusters, n_particles]
  rho_array <- x$rho
  weights <- x$importance_weights
  
  # Get dimensions
  dims <- dim(rho_array)
  n_items <- dims[1]
  n_clusters <- dims[2]
  n_particles <- dims[3]
  
  # Handle item selection
  if (is.null(items) && n_items > 5) {
    message("Items not provided by user. Picking 5 at random.")
    items <- sample.int(n_items, 5)
  } else if (is.null(items)) {
    items <- seq_len(n_items)
  } else {
    # Validate items
    if (is.character(items)) {
      stop("Item names are not currently supported. Please use item indices (1 to ", n_items, ").")
    }
    if (!all(items %in% seq_len(n_items))) {
      stop("Invalid item indices. Items must be between 1 and ", n_items, ".")
    }
  }
  
  # Create data frame for plotting
  plot_data_list <- vector("list", length(items) * n_clusters * n_items)
  idx <- 1
  
  for (item_idx in items) {
    for (cluster in seq_len(n_clusters)) {
      # Get rankings for this item and cluster across all particles
      rankings <- rho_array[item_idx, cluster, ]
      
      # Compute weighted probability for each rank
      for (rank in seq_len(n_items)) {
        # Weight of particles where this item has this rank
        prob <- sum(weights[rankings == rank]) / sum(weights)
        
        plot_data_list[[idx]] <- data.frame(
          item = paste0("Item ", item_idx),
          cluster = if (n_clusters > 1) paste0("Cluster ", cluster) else "All",
          rank = rank,
          probability = prob
        )
        idx <- idx + 1
      }
    }
  }
  
  plot_data <- do.call(rbind, plot_data_list)
  
  # Create bar chart
  p <- ggplot2::ggplot(plot_data, ggplot2::aes(x = factor(rank), y = probability)) +
    ggplot2::geom_col(fill = "steelblue") +
    ggplot2::xlab("Rank") +
    ggplot2::ylab("Posterior Probability") +
    ggplot2::theme_minimal()
  
  # Add faceting
  if (n_clusters == 1) {
    p <- p + ggplot2::facet_wrap(~ item)
  } else {
    p <- p + ggplot2::facet_wrap(~ cluster + item)
  }
  
  p
}
