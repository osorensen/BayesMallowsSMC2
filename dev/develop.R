devtools::load_all()
data <- pairwise_preferences
hyperparameters <- set_hyperparameters(n_items = 5)
smc_options <- set_smc_options(n_particles = 500, n_particle_filters = 100, verbose = TRUE, resampler = "residual")
ret <- compute_sequentially(data, hyperparameters, smc_options)

plot(ret$alpha_trace[1,])
hist(ret$alpha)
