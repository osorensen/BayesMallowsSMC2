devtools::load_all()
data <- pairwise_preferences
hyperparameters <- set_hyperparameters(n_items = 5)
smc_options <- set_smc_options(n_particles = 50, n_particle_filters = 20, verbose = TRUE)
ret <- compute_sequentially(data, hyperparameters, smc_options)

plot(ret$alpha_trace[1,])

