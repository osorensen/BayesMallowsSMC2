devtools::load_all()
data <- subset(complete_rankings_mixtures, user < 100)
hyperparameters <- set_hyperparameters(n_items = 5, n_clusters = 2)
smc_options <- set_smc_options(n_particles = 10, n_particle_filters = 1, verbose = TRUE, resampler = "residual")
ret <- compute_sequentially(data, hyperparameters, smc_options)

plot(ret$alpha_trace[1,], type = "l", ylim = c(0, 2))
lines(ret$alpha_trace[2, ], col = "red")

par(mfrow = c(1, 2))
hist(ret$tau[1, ])
hist(ret$tau[2, ])
dev.off()


apply(ret$rho, c(1, 2), mean)
hist(ret$tau[1,])
hist(ret$tau[2, ])

apply(ret$tau, 1, mean)
