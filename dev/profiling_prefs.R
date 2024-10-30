library(Rgperftools)
library(tidyverse)
library(furrr)
library(glue)

# Force devtools to compile without debug flags for optimal performance profiling
library(pkgbuild)
flags <- pkgbuild::compiler_flags(debug=FALSE)
new_compiler_flags <- function(debug=FALSE) {flags}
assignInNamespace('compiler_flags', new_compiler_flags, 'pkgbuild')

# Load your library
devtools::load_all()

set.seed(1)
n_items <- 5
n <- 50
alpha <- .4
rankings <- BayesMallows::sample_mallows(1:n_items, alpha * n_items, n_samples = n, thinning = 1000)
colnames(rankings) <- paste0("item", 1:n_items)

dat <- tibble(
  timepoint = seq_len(n),
  user = seq_len(n)
) %>%
  bind_cols(as.data.frame(rankings)) %>%
  pivot_longer(cols = starts_with("item")) %>%
  inner_join(x = ., y = ., by = c("timepoint", "user"), relationship = "many-to-many") %>%
  filter(name.x != name.y) %>%
  group_by(timepoint, user) %>%
  slice_sample(n = 4) %>%
  ungroup() %>%
  mutate(
    top_item = if_else(value.x < value.y, name.x, name.y),
    bottom_item = if_else(value.x < value.y, name.y, name.x)
  ) %>%
  select(timepoint, user, top_item, bottom_item) %>%
  mutate(
    across(c("top_item", "bottom_item"), ~ as.integer(str_extract(., "[:digit:]+$")))
  )

users <- unique(dat$user)

dir.create("dev/tmp")
plan("multisession")
num_topological_sorts <- future_map_int(users, function(u) {
  rels <- as.matrix(subset(dat, user == u)[, c("top_item", "bottom_item")])

  precompute_topological_sorts(
    rels, n_items = n_items, output_directory = glue("dev/tmp/user{u}"), 1)
}, .options = furrr_options(seed = TRUE))
plan("default")

file_count <- map_dfr(list.files("dev/tmp", full.names = TRUE), function(f) {
  tibble(
    count = length(list.files(f)),
    user = as.integer(str_extract(f, "[:digit:]+$"))
  )
}) %>%
  arrange(user) %>%
  pull(count)

data <- dat
hyperparameters <- set_hyperparameters(n_items = n_items)
smc_options <- set_smc_options(n_particles = 100, n_particle_filters = 10,
                               max_particle_filters = 100,
                               max_rejuvenation_steps = 1, verbose = FALSE)
topological_sorts_directory <- "dev/tmp"
num_topological_sorts <- num_topological_sorts

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
  if(is.null(file_count)) {
    stop("file_count must be provided with preference data.")
  }
  input_timeseries <- split(data, f = ~ timepoint) |>
    lapply(split, f = ~ user) |>
    lapply(function(x) lapply(x, function(y) as.matrix(y[preference_columns])))
  attr(input_timeseries, "type") <- "pairwise preferences"
  attr(input_timeseries, "topological_sorts_directory") <- topological_sorts_directory
  names(num_topological_sorts) <- names(file_count) <- as.character(lapply(input_timeseries, names))
  attr(input_timeseries, "num_topological_sorts") <- num_topological_sorts
  attr(input_timeseries, "file_count") <- file_count
} else {
  stop("Something wrong with data")
}

attr(input_timeseries, "updated_users") <- max(table(data$user)) > 1 &&
  attr(input_timeseries, "type") != "pairwise preferences"


# Collect profiling data
start_profiler("/tmp/profile.out")
set.seed(2)
ret <- run_smc(input_timeseries, hyperparameters, smc_options)
stop_profiler()


hist(ret$alpha)
