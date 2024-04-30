test_that("compute_sequentially works with complete data", {
  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)

  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1,
                                  resampler = "residual")
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)

  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1,
                                  resampler = "stratified")
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)

  set.seed(2)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1,
                                  resampler = "systematic")
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)
})

test_that("compute_sequentially works with partial data", {
  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100)
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, resampler = "residual")
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, resampler = "stratified")
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, resampler = "systematic")
  )
  expect_gt(mean(mod$alpha), .7)
  expect_lt(mean(mod$alpha), 1.3)
})

test_that("compute_sequentially works with pairwise preference data", {

})
