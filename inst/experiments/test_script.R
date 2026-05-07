devtools::load_all()
library(BayesMallows)
library(tidyverse)
library(coda)

set.seed(42)
component1 <- BayesMallows::sample_mallows(rho0 = 1:5, alpha0 = 1, n_samples = 250, thinning = 100)
component2 <- BayesMallows::sample_mallows(rho0 = 5:1, alpha0 = 3, n_samples = 250, thinning = 100)
rankings <- rbind(component1, component2)
colnames(rankings) <- paste0("item", 1:5)

sim_data <- as_tibble(rankings) %>%
  slice_sample(prop = 1) %>%
  mutate(timepoint = row_number(), user = row_number()) %>%
  select(timepoint, user, everything()) %>%
  as.data.frame()

hyperparams <- set_hyperparameters(n_items = 5, n_clusters = 2)

cat("Running Standard...\n")
opts_std <- BayesMallowsSMC2::set_smc_options(n_particles = 100, n_particle_filters = 10, max_rejuvenation_steps = 10, use_backward_simulation = FALSE)
mod_std <- compute_sequentially(sim_data, hyperparameters = hyperparams, smc_options = opts_std)

cat("Running BSi...\n")
opts_bsi <- BayesMallowsSMC2::set_smc_options(n_particles = 100, n_particle_filters = 10, max_rejuvenation_steps = 10, use_backward_simulation = TRUE)
mod_bsi <- compute_sequentially(sim_data, hyperparameters = hyperparams, smc_options = opts_bsi)

saveRDS(list(mod_std = mod_std, mod_bsi = mod_bsi), "inst/experiments/results.rds")
cat("Done.\n")
