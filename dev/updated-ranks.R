devtools::load_all()
library(tidyverse)

n_items <- 8
n <- 50
alpha <- .4
rankings <- BayesMallows::sample_mallows(1:n_items, alpha * n_items, n_samples = n, thinning = 1000)
colnames(rankings) <- paste0("item", 1:n_items)

dd1 <- rankings
dd1[dd1 > 2] <- NA
dd2 <- rankings
dd2[dd2 > 5] <- NA

dd <- as.data.frame(rbind(dd1, dd2))
dd$timepoint <- seq_len(nrow(dd))
dd$user <- c(1:n, 1:n)
#dd <- dd[51:100, ]

mod <- compute_sequentially(
  dd[c(1:40, 60), ],
  hyperparameters = set_hyperparameters(n_items = n_items),
  smc_options = set_smc_options(n_particles = 500, max_rejuvenation_steps = 5, verbose = TRUE,
                                n_particle_filters = 50, max_particle_filters = 500)
)

hist(mod$alpha)

weighted.mean(mod$alpha[1, ], w = mod$importance_weights)

apply(mod$rho, c(1, 2), mean)


