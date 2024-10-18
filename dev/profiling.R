library(Rgperftools)
library(tidyverse)

# Force devtools to compile without debug flags for optimal performance profiling
library(pkgbuild)
flags <- pkgbuild::compiler_flags(debug=FALSE)
new_compiler_flags <- function(debug=FALSE) {flags}
assignInNamespace('compiler_flags', new_compiler_flags, 'pkgbuild')

# Load your library
devtools::load_all()

n_items <- 10
alpha <- .3
n_users <- 50
n_timepoints <- n_users
set.seed(1)
rankings <- BayesMallows::sample_mallows(
  rho0 = seq_len(n_items), alpha0 = alpha * n_items, n_samples = n_users,
  thinning = 1000)
colnames(rankings) <- paste0("item", seq_len(n_items))
rankings[rankings > 3] <- NA

complete_rankings <- tibble(
  timepoint = rep(seq_len(n_timepoints), each = 1),
  user = seq_len(n_users)) %>%
  bind_cols(rankings) %>%
  as.data.frame()

data <- complete_rankings
hyperparameters <- set_hyperparameters(n_items = n_items)
smc_options <- set_smc_options(n_particles = 300, n_particle_filters = 100,
                               max_rejuvenation_steps = 5,
                               max_particle_filters = 1000,
                               resampler = "multinomial")
topological_sorts_directory = NULL
num_topological_sorts = NULL

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
} else if(sum(preference_columns) == 2) {
  if(is.null(topological_sorts_directory)) {
    stop("topological_sorts_directory must be provided with preference data.")
  }
  if(is.null(num_topological_sorts)) {
    stop("num_topological_sorts must be provided with preference data.")
  }
  input_timeseries <- split(data, f = ~ timepoint) |>
    lapply(split, f = ~ user) |>
    lapply(function(x) lapply(x, function(y) as.matrix(y[preference_columns])))
  attr(input_timeseries, "type") <- "pairwise preferences"
  attr(input_timeseries, "topological_sorts_directory") <- topological_sorts_directory
  names(num_topological_sorts) <- as.character(lapply(input_timeseries, names))
  attr(input_timeseries, "num_topological_sorts") <- num_topological_sorts
} else {
  stop("Something wrong with data")
}

attr(input_timeseries, "updated_users") <-
  if(length(unique(data$timepoint)) < length(unique(data$user))) {
    TRUE } else { FALSE }

# Collect profiling data
start_profiler("/tmp/profile.out")
set.seed(2)
ret <- run_smc(input_timeseries, hyperparameters, smc_options)
stop_profiler()
