# dd1 <- complete_rankings[1:20, ]
# dd1[, -(1:2)][dd1[, -(1:2)] > 1] <- NA
# dd2 <- complete_rankings[1:20, ]
# dd2[, -(1:2)][dd2[, -(1:2)] > 2] <- NA
# dd <- rbind(dd1, dd2)
#
# dd$timepoint <- seq_along(dd$timepoint)

test_that("compute_sequentially works with new and updated users", {
  # set.seed(2)
  # mod <- compute_sequentially(
  #   dd,
  #   hyperparameters = set_hyperparameters(n_items = 5),
  #   smc_options = set_smc_options(
  #     n_particles = 1000, n_particle_filters = 1, verbose = TRUE,
  #     max_particle_filters = 1000, max_rejuvenation_steps = 1)
  # )
  # alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  # expect_gt(alpha_hat, .94)
  # expect_lt(alpha_hat, 1.1)
})
