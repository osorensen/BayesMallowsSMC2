test_that("compute_sequentially works with pairwise preference data", {

  set.seed(2)
  mod <- compute_sequentially(
    subset(pairwise_preferences, timepoint < 50),
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 200, resampler = "residual")
  )
  expect_gt(mean(mod$alpha), .75)
  expect_lt(mean(mod$alpha), 1.25)

})
