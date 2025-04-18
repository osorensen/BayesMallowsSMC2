test_that("compute_sequentially works with partial data", {
  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100, n_particle_filters = 10,
      max_particle_filters = 30, max_rejuvenation_steps = 5)
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .95)
  expect_lt(alpha_hat, 1.06)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100, n_particle_filters = 10, max_particle_filters = 30,
      resampler = "residual", max_rejuvenation_steps = 5)
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .99)
  expect_lt(alpha_hat, 1.06)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      n_particle_filters = 10,
      max_particle_filters = 30,
      resampler = "stratified",
      max_rejuvenation_steps = 5)
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .94)
  expect_lt(alpha_hat, 1.05)

  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      n_particle_filters = 10,
      max_particle_filters = 30,
      resampler = "systematic",
      max_rejuvenation_steps = 5)
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .99)
  expect_lt(alpha_hat, 1.08)
})

test_that("compute_sequentially works with partial data and pseudolikelihood proposal", {
  set.seed(2)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 50,
      n_particle_filters = 10,
      max_particle_filters = 20,
      max_rejuvenation_steps = 5,
      latent_rank_proposal = "pseudo")
  )
  alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
  expect_gt(alpha_hat, .98)
  expect_lt(alpha_hat, 1.06)

})
