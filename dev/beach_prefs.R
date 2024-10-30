library(BayesMallowsSMC2)
library(tidyverse)
library(furrr)
library(glue)

n_items <- 5
dat <- BayesMallows::beach_preferences %>%
  filter(top_item %in% seq_len(n_items), bottom_item %in% seq_len(n_items)) %>%
  as_tibble() %>%
  rename(user = assessor) %>%
  mutate(timepoint = user) %>%
  select(timepoint, user, top_item, bottom_item) %>%
  as.data.frame()

users <- unique(dat$user)

plan("multisession")
future_walk(users, function(u) {
  rels <- as.matrix(subset(dat, timepoint == 1 & user == u)[, c("top_item", "bottom_item")])

  precompute_topological_sorts(
    rels, n_items = n_items, output_directory = glue("dev/tmp/user{u}"), 1)
}, .options = furrr_options(seed = TRUE))
plan("default")

mod <- compute_sequentially(
  data = dat,
  hyperparameters = set_hyperparameters(n_items = n_items),
  smc_options = set_smc_options(n_particles = 3, n_particle_filters = 3,
                                max_rejuvenation_steps = 5, verbose = TRUE)
)
