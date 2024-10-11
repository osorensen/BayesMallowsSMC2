devtools::load_all()
library(tidyverse)

n_items <- 5
set.seed(1)

rankings <- BayesMallows::sample_mallows(1:n_items, .5 * n_items, n_samples = 2, thinning = 1000)
colnames(rankings) <- paste0("item", 1:n_items)

dd1 <- rankings
dd1[dd1 > 2] <- NA
dd2 <- rankings
dd2[dd2 > 3] <- NA

dd <- as.data.frame(rbind(dd1, dd2))
dd$timepoint <- seq_len(nrow(dd))
dd$user <- c(1:2, 1:2)

set.seed(2)
mod <- compute_sequentially(
  dd,
  hyperparameters = set_hyperparameters(n_items = n_items),
  smc_options = set_smc_options(n_particles = 1, max_rejuvenation_steps = 5, verbose = TRUE,
                                n_particle_filters = 1, max_particle_filters = 1,
                                resampling_threshold = 0)
)

 hist(mod$alpha)

weighted.mean(mod$alpha[1, ], w = mod$importance_weights)



