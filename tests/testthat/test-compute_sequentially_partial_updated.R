dd <- complete_rankings[1:40, ]
dd[, -(1:2)][dd[, -(1:2)] > 3] <- NA
dd <- rbind(dd[1:20, ], complete_rankings[1:20, ], dd[21:40, ], complete_rankings[21:40, ])

dd$timepoint <- seq_along(dd$timepoint)

test_that("compute_sequentially works with new and updated users", {
  set.seed(2)
  mod <- compute_sequentially(
    dd,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 200, n_particle_filters = 100, max_rejuvenation_steps = 5, verbose = FALSE,
      max_particle_filters = 100)
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .94)
  expect_lt(alpha_hat, 1.1)
})
