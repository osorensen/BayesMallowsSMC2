library(BayesMallowsSMC2)
library(tidyverse)

dat <- BayesMallows::beach_preferences %>%
  as_tibble() %>%
  rename(user = assessor) %>%
  mutate(timepoint = user) %>%
  select(timepoint, user, top_item, bottom_item) %>%
  as.data.frame()

rels <- as.matrix(subset(dat, timepoint == 1 & user == 1)[, c("top_item", "bottom_item")])
ss <- ""
for(i in seq_len(nrow(rels))) {
  ss <- paste(paste0(ss, "[", rels[i, 1], ",", rels[i, 2], "]"), ",")
}

precompute_topological_sorts(
  rels,
  n_items = 15,
  output_directory = "tmp/user1",
  .01)

mod <- compute_sequentially(
  data = dat,
  hyperparameters = set_hyperparameters(n_items = 15),
  smc_options = set_smc_options(n_particles = 3, n_particle_filters = 3,
                                max_rejuvenation_steps = 5, verbose = TRUE)
)
