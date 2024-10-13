devtools::load_all()
library(tidyverse)

n_items <- 10

n <- 200
rankings <- BayesMallows::sample_mallows(1:n_items, .3 * n_items, n_samples = n, thinning = 1000)
colnames(rankings) <- paste0("item", 1:n_items)

dd1 <- rankings
dd1[dd1 > 1] <- NA
dd2 <- rankings
dd2[dd2 > 3] <- NA
dd3 <- rankings
dd3[dd3 > 5] <- NA

mm <- BayesMallows::compute_mallows(
  data = BayesMallows::setup_rank_data(rbind(dd1, dd2, dd3)),
  compute_options = BayesMallows::set_compute_options(nmc = 3000, burnin = 500)
)
BayesMallows::compute_posterior_intervals(mm)

dd <- as.data.frame(rbind(dd1, dd2, dd3))
dd$timepoint <- seq_len(nrow(dd))
dd$user <- c(1:n, 1:n, 1:n)

mod <- compute_sequentially(
  dd,
  hyperparameters = set_hyperparameters(n_items = n_items),
  smc_options = set_smc_options(n_particles = 1000, max_rejuvenation_steps = 5, verbose = TRUE,
                                n_particle_filters = 20, max_particle_filters = 200)
)

hist(mod$alpha)

weighted.mean(mod$alpha[1, ], w = mod$importance_weights)

apply(mod$rho, c(1, 2), mean)


