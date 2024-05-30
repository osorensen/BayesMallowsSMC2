library(BayesMallowsSMC2)
library(tidyverse)

dat <- BayesMallows::beach_preferences %>%
  as_tibble() %>%
  rename(user = assessor) %>%
  mutate(timepoint = user) %>%
  select(timepoint, user, top_item, bottom_item) %>%
  as.data.frame()

mod <- compute_sequentially(
  data = dat,
  hyperparameters = set_hyperparameters(n_items = 15),
  smc_options = set_smc_options(n_particles = 3, n_particle_filters = 3,
                                max_rejuvenation_steps = 5, verbose = TRUE)
)
