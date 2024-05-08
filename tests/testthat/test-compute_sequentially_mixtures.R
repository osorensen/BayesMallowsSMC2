test_that("compute_sequentially works for mixtures", {
  set.seed(2)
  mod <- compute_sequentially(
    mixtures,
    hyperparameters = set_hyperparameters(n_items = 5, n_clusters = 2),
    smc_options = set_smc_options(n_particles = 200, n_particle_filters = 1)
  )
  means <- apply(mod$alpha, 1, mean)
  expect_lt(means[[1]], 2.1)
  expect_gt(means[[1]], 1.9)
  expect_lt(means[[2]], 1.1)
  expect_gt(means[[2]], .9)

  set.seed(2)
  mod <- compute_sequentially(
    mixtures,
    hyperparameters = set_hyperparameters(n_items = 5, n_clusters = 2),
    smc_options = set_smc_options(n_particles = 200, n_particle_filters = 1,
                                  resampler = "residual")
  )
  means <- apply(mod$alpha, 1, mean)
  expect_lt(means[[1]], 2.1)
  expect_gt(means[[1]], 1.9)
  expect_lt(means[[2]], 1.1)
  expect_gt(means[[2]], .9)

  set.seed(2)
  mod <- compute_sequentially(
    mixtures,
    hyperparameters = set_hyperparameters(n_items = 5, n_clusters = 2),
    smc_options = set_smc_options(n_particles = 200, n_particle_filters = 1,
                                  resampler = "stratified")
  )
  means <- apply(mod$alpha, 1, mean)
  expect_lt(means[[1]], 2.1)
  expect_gt(means[[1]], 1.9)
  expect_lt(means[[2]], 1.1)
  expect_gt(means[[2]], .9)

  set.seed(2)
  mod <- compute_sequentially(
    mixtures,
    hyperparameters = set_hyperparameters(n_items = 5, n_clusters = 2),
    smc_options = set_smc_options(n_particles = 200, n_particle_filters = 1,
                                  resampler = "systematic")
  )
  means <- apply(mod$alpha, 1, mean)
  expect_lt(means[[1]], 2.1)
  expect_gt(means[[1]], 1.9)
  expect_lt(means[[2]], 1.1)
  expect_gt(means[[2]], .9)
})
