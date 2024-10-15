devtools::load_all()
library(tidyverse)

n_items <- 5
set.seed(1)
n <- 20
rankings <- BayesMallows::sample_mallows(1:n_items, 1 * n_items, n_samples = n, thinning = 1000)
colnames(rankings) <- paste0("item", 1:n_items)

dd1 <- rankings
dd1[dd1 > 2] <- NA
dd2 <- rankings
dd2[dd2 > 3] <- NA

mm <- BayesMallows::compute_mallows(
  data = BayesMallows::setup_rank_data(rbind(dd1, dd2)),
  compute_options = BayesMallows::set_compute_options(nmc = 10000, burnin = 500)
)
BayesMallows::compute_posterior_intervals(mm)

dd <- as.data.frame(rbind(dd1, dd2))
dd$timepoint <- seq_len(nrow(dd))
dd$user <- c(1:n, 1:n)

mod <- compute_sequentially(
  dd,
  hyperparameters = set_hyperparameters(n_items = n_items),
  smc_options = set_smc_options(n_particles = 100, max_rejuvenation_steps = 5, verbose = TRUE,
                                n_particle_filters = 20, max_particle_filters = 50)
)

hist(mod$alpha)

weighted.mean(mod$alpha[1, ], w = mod$importance_weights)

apply(mod$rho, c(1, 2), mean)


