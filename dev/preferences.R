library(BayesMallowsSMC2)
library(tidyverse)
library(furrr)
library(glue)

Rcpp::sourceCpp("dev/read_arma_files.cpp")

set.seed(1)
n_items <- 5
n <- 20
alpha <- 1
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


tmp <- dat %>%
  select(user, bottom_item, top_item) %>%
  rename(assessor = user) %>%
  as.data.frame()
mm <- BayesMallows::compute_mallows(
  data = BayesMallows::setup_rank_data(preferences = tmp),
  compute_options = BayesMallows::set_compute_options(nmc = 5000, burnin = 1000)
)
as.numeric(BayesMallows::compute_posterior_intervals(mm)[, "mean"]) / n_items

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

mod <- compute_sequentially(
  data = dat,
  hyperparameters = set_hyperparameters(n_items = n_items),
  smc_options = set_smc_options(n_particles = 100, n_particle_filters = 20,
                                max_particle_filters = 200,
                                max_rejuvenation_steps = 5, verbose = TRUE,
                                trace = TRUE, trace_directory = "dev/trace"),
  topological_sorts_directory = "dev/tmp",
  num_topological_sorts = num_topological_sorts,
  file_count = file_count
)


weighted.mean(as.numeric(mod$alpha), mod$importance_weights)

alpha_files <- list.files("dev/trace/", pattern = "alpha", full.names = TRUE)
weight_files <- list.files("dev/trace/", pattern = "weights", full.names = TRUE)

alpha_trace <- map2_dfr(alpha_files, weight_files, function(af, wf) {
  alpha <- as.numeric(read_mat(af))
  weight <- exp(as.numeric(read_mat(wf)))

  tibble(
    timepoint = as.integer(str_extract(af, "[:digit:]+")),
    alpha = weighted.mean(x = alpha, w = weight)
  )
})


ggplot(alpha_trace, aes(x = timepoint, y = alpha)) +
  geom_line()
