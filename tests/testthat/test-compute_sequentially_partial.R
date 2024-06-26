test_that("compute_sequentially works with partial data", {
  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, max_rejuvenation_steps = 5)
  )
  expect_gt(mean(mod$alpha), .95)
  expect_lt(mean(mod$alpha), 1.05)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, resampler = "residual", max_rejuvenation_steps = 5)
  )
  expect_gt(mean(mod$alpha), .95)
  expect_lt(mean(mod$alpha), 1.05)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, resampler = "stratified", max_rejuvenation_steps = 5)
  )
  expect_gt(mean(mod$alpha), .95)
  expect_lt(mean(mod$alpha), 1.05)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, resampler = "systematic", max_rejuvenation_steps = 5)
  )
  expect_gt(mean(mod$alpha), .95)
  expect_lt(mean(mod$alpha), 1.05)
})
