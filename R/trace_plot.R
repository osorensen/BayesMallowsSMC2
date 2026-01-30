#' Create Trace Plots for BayesMallowsSMC2 Objects
#'
#' Visualize the timeseries dynamics of the alpha and tau parameters across
#' timepoints. This function creates trace plots similar to Figure 4 (left) in
#' \insertRef{10.1214/25-BA1564}{BayesMallowsSMC2}.
#'
#' @param x An object of class `BayesMallowsSMC2`, returned from
#'   [compute_sequentially()] with `trace = TRUE` in [set_smc_options()].
#' @param parameter Character string defining the parameter to plot. Available
#'   options are `"alpha"` (default) and `"tau"`.
#' @param ... Other arguments (currently unused).
#'
#' @return A ggplot object showing the evolution of the parameter over time.
#'   For each timepoint, the plot shows:
#'   \itemize{
#'     \item The weighted mean (solid line)
#'     \item The weighted 0.025 and 0.975 quantiles (shaded area representing
#'       the 95% credible interval)
#'   }
#'
#' @details
#' This function requires that the model was fitted with `trace = TRUE` in the
#' `smc_options`. The trace contains the parameter values at each timepoint,
#' which allows visualization of how the posterior distribution evolves as more
#' data arrives sequentially.
#'
#' For mixture models (multiple clusters), separate trace plots are created for
#' each cluster using faceting.
#'
#' The shaded area represents the 95% credible interval (from 2.5% to 97.5%
#' quantiles) of the posterior distribution at each timepoint, computed using
#' the importance weights from the SMC algorithm.
#'
#' @export
#'
#' @references
#' \insertRef{10.1214/25-BA1564}{BayesMallowsSMC2}
#'
#' @examples
#' \donttest{
#' # Fit a model with trace enabled
#' mod <- compute_sequentially(
#'   complete_rankings,
#'   hyperparameters = set_hyperparameters(n_items = 5),
#'   smc_options = set_smc_options(
#'     n_particles = 100,
#'     n_particle_filters = 1,
#'     trace = TRUE
#'   )
#' )
#'
#' # Create trace plot for alpha (default)
#' trace_plot(mod)
#' }
trace_plot <- function(x, parameter = "alpha", ...) {
  # Validate parameter
  parameter <- match.arg(parameter, c("alpha", "tau"))

  # Basic validation
  if (!inherits(x, "BayesMallowsSMC2")) {
    stop("x must be an object of class 'BayesMallowsSMC2'")
  }

  # Check if trace was enabled
  trace_field <- paste0(parameter, "_traces")
  if (!trace_field %in% names(x)) {
    stop("Trace data not found. Please run compute_sequentially with trace = TRUE in set_smc_options().")
  }

  traces <- x[[trace_field]]

  if (length(traces) == 0) {
    stop("Trace data not found. Please run compute_sequentially with trace = TRUE in set_smc_options().")
  }

  # Check for importance weights trace
  if (!"log_importance_weights_traces" %in% names(x)) {
    stop("Importance weights trace not found. This should not happen if trace = TRUE was used.")
  }

  log_weights_traces <- x$log_importance_weights_traces

  if (parameter == "alpha") {
    plot_trace_alpha_tau(traces, log_weights_traces, parameter_name = "alpha",
                         parameter_label = expression(alpha))
  } else if (parameter == "tau") {
    plot_trace_alpha_tau(traces, log_weights_traces, parameter_name = "tau",
                         parameter_label = expression(tau))
  }
}

# Avoid R CMD check NOTE about undefined global variables used in ggplot2::aes()
utils::globalVariables(c("timepoint", "mean", "lower", "upper", "cluster"))


# Internal function to plot trace for alpha or tau parameter
# @param traces List of matrices, one per timepoint. Each matrix is [n_clusters x n_particles]
# @param log_weights_traces List of vectors, one per timepoint. Each vector is length n_particles
# @param parameter_name Character string, name of the parameter
# @param parameter_label Expression for axis label
plot_trace_alpha_tau <- function(traces, log_weights_traces, parameter_name,
                                 parameter_label) {
  n_timepoints <- length(traces)

  # Get dimensions from first trace
  # Need to infer n_clusters and n_particles from the trace
  first_trace <- traces[[1]]

  # If trace is a vector, need to infer dimensions
  if (is.vector(first_trace)) {
    # Get n_particles from log_weights
    n_particles <- length(log_weights_traces[[1]])

    # If trace is a vector, infer n_clusters from its length
    trace_length <- length(first_trace)
    if (trace_length %% n_particles == 0) {
      n_clusters <- trace_length %/% n_particles
    } else {
      stop(sprintf("Trace length (%d) is not divisible by n_particles (%d). ",
                   trace_length, n_particles),
           "This indicates inconsistent dimensions in the trace data.")
    }

    # Convert all traces to matrices [n_clusters x n_particles]
    # The C++ code stores traces as: alpha is [n_clusters x n_particles] matrix per timepoint
    # When passed to R as vector, elements are in column-major order:
    # cluster1_particle1, cluster2_particle1, cluster1_particle2, cluster2_particle2, ...
    traces <- lapply(traces, function(t) {
      matrix(t, nrow = n_clusters, ncol = n_particles, byrow = FALSE)
    })
    first_trace <- traces[[1]]
  } else if (is.matrix(first_trace)) {
    n_clusters <- nrow(first_trace)
    n_particles <- ncol(first_trace)
  } else {
    stop("Trace elements must be vectors or matrices")
  }

  # Create data frame for plotting
  plot_data_list <- vector("list", n_timepoints * n_clusters)
  idx <- 1

  for (t in seq_len(n_timepoints)) {
    param_matrix <- traces[[t]]
    log_weights <- log_weights_traces[[t]]

    # Normalize weights
    weights <- exp(log_weights - max(log_weights))
    weights <- weights / sum(weights)

    for (cluster in seq_len(n_clusters)) {
      param_values <- param_matrix[cluster, ]

      # Compute weighted statistics
      weighted_mean <- stats::weighted.mean(param_values, weights)
      weighted_quantiles <- weighted_quantile(param_values, weights,
                                              probs = c(0.025, 0.975))

      plot_data_list[[idx]] <- data.frame(
        timepoint = t,
        mean = weighted_mean,
        lower = weighted_quantiles[1],
        upper = weighted_quantiles[2],
        cluster = if (n_clusters > 1) paste0("Cluster ", cluster) else "All"
      )
      idx <- idx + 1
    }
  }

  plot_data <- do.call(rbind, plot_data_list)

  # Create line plot with credible interval
  p <- ggplot2::ggplot(plot_data, ggplot2::aes(x = timepoint, y = mean)) +
    ggplot2::geom_ribbon(ggplot2::aes(ymin = lower, ymax = upper),
                        alpha = 0.3, fill = "steelblue") +
    ggplot2::geom_line(color = "darkblue", linewidth = 1) +
    ggplot2::xlab("Timepoint") +
    ggplot2::ylab(parameter_label) +
    ggplot2::theme_minimal() +
    ggplot2::theme(
      panel.grid.minor = ggplot2::element_blank()
    )

  # Add faceting if multiple clusters
  if (n_clusters > 1) {
    p <- p + ggplot2::facet_wrap(~ cluster, scales = "free_y")
  }

  p
}


# Internal helper function to compute weighted quantiles
# @param x Numeric vector of values
# @param weights Numeric vector of weights
# @param probs Numeric vector of probabilities
# @return Numeric vector of quantiles
weighted_quantile <- function(x, weights, probs) {
  # Sort x and weights by x
  ord <- order(x)
  x_sorted <- x[ord]
  weights_sorted <- weights[ord]

  # Compute cumulative weights
  cum_weights <- cumsum(weights_sorted) / sum(weights_sorted)

  # Find quantiles
  quantiles <- numeric(length(probs))
  for (i in seq_along(probs)) {
    # Find first position where cumulative weight exceeds prob
    idx <- which(cum_weights >= probs[i])[1]
    if (is.na(idx)) {
      idx <- length(x_sorted)
    }
    quantiles[i] <- x_sorted[idx]
  }

  quantiles
}
