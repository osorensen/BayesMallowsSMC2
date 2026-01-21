test_that("compute_sequentially works with complete data", {
  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  expect_s3_class(mod, "BayesMallowsSMC2")
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .99)
  expect_lt(alpha_hat, 1.05)

  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 200, n_particle_filters = 1,
                                  resampler = "residual")
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .99)
  expect_lt(alpha_hat, 1.06)

  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1,
                                  resampler = "stratified")
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .99)
  expect_lt(alpha_hat, 1.1)

  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1,
                                  resampler = "systematic")
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .99)
  expect_lt(alpha_hat, 1.06)
})
