devtools::load_all()

dd1 <- complete_rankings[1:10, ]
dd1[, -(1:2)][dd1[, -(1:2)] > 1] <- NA

dd2 <- complete_rankings[1:10, ]
dd2[, -(1:2)][dd2[, -(1:2)] > 2] <- NA

dd3 <- complete_rankings[1:10, ]
dd3[, -(1:2)][dd3[, -(1:2)] > 3] <- NA

dd <- rbind(dd1, dd2, dd3)
#dd <- dd[order(dd$user), ]
dd$timepoint <- seq_len(nrow(dd))

m <- compute_sequentially(
  data = dd[1:15, ],
  hyperparameters = set_hyperparameters(n_items = 5),
  smc_options = set_smc_options(n_particles = 2, n_particle_filters = 2,
                                max_particle_filters = 2, max_rejuvenation_steps = 5,
                                verbose = TRUE)
)

length(unique(as.numeric(m$alpha)))
hist(m$alpha)
