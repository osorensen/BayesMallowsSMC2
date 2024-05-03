devtools::load_all()
data <- subset(partial_rankings_updated, user < 3)

hyperparameters <- set_hyperparameters(n_items = 5)
smc_options <- set_smc_options(verbose = FALSE, n_particles = 1, n_particle_filters = 1, resampler = "residual")


mod <- compute_sequentially(data, hyperparameters, smc_options)

hist(mod$alpha)
