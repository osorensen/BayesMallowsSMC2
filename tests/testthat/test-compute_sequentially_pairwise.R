test_that("compute_sequentially works with pairwise preference data", {
  skip()
  set.seed(2)
  mod <- compute_sequentially(
    pairwise_preferences,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, verbose = TRUE)
  )
  expect_gt(mean(mod$alpha), .14)
  expect_lt(mean(mod$alpha), .26)

  set.seed(2)
  mod <- compute_sequentially(
    pairwise_preferences,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 200, resampler = "residual")
  )
  expect_gt(mean(mod$alpha), .14)
  expect_lt(mean(mod$alpha), .26)

  set.seed(2)
  mod <- compute_sequentially(
    pairwise_preferences,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 100, resampler = "stratified")
  )
  expect_gt(mean(mod$alpha), .14)
  expect_lt(mean(mod$alpha), .26)

  set.seed(2)
  mod <- compute_sequentially(
    pairwise_preferences,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 100, resampler = "systematic")
  )
  expect_gt(mean(mod$alpha), .14)
  expect_lt(mean(mod$alpha), .26)
})
