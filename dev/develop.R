devtools::load_all()
data <- subset(complete_rankings_heterogeneous)
#data$timepoint <- 1:nrow(data)
hyperparameters <- set_hyperparameters(n_items = 5)
smc_options <- set_smc_options(verbose = FALSE, n_particle_filters = 1, resampler = "residual")
mod <- compute_sequentially(data, hyperparameters, smc_options)

hist(mod$alpha)
