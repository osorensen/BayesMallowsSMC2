devtools::load_all()
dd <- complete_rankings
dd[, -(1:2)][dd[, -(1:2)] > 3] <- NA
dd <- rbind(dd, complete_rankings)
dd$timepoint <- seq_along(dd$timepoint)

m <- compute_sequentially(
  dd,
  hyperparameters = set_hyperparameters(n_items = 5),
  smc_options = set_smc_options(n_particles = 100, n_particle_filters = 50,
                                verbose = TRUE)
)

alpha_hat <- weighted.mean(x = as.numeric(m$alpha), w = m$importance_weights)
m$n_particle_filters
