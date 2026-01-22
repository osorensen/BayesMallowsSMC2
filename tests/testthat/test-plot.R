test_that("plot.BayesMallowsSMC2 works with alpha parameter", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test that plot function runs without error for alpha
  expect_no_error(plot(mod, parameter = "alpha"))
  expect_no_error(plot(mod))  # alpha is default
  
  # Check that it returns a ggplot object
  p <- plot(mod, parameter = "alpha")
  expect_s3_class(p, "ggplot")
})

test_that("plot.BayesMallowsSMC2 works with tau parameter", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test that plot function runs without error for tau
  expect_no_error(plot(mod, parameter = "tau"))
  
  # Check that it returns a ggplot object
  p <- plot(mod, parameter = "tau")
  expect_s3_class(p, "ggplot")
})

test_that("plot.BayesMallowsSMC2 works with rho parameter", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test that plot function runs without error for rho
  # Should not give message since we have exactly 5 items
  expect_no_message(plot(mod, parameter = "rho"))
  
  # Check that it returns a ggplot object
  p <- plot(mod, parameter = "rho")
  expect_s3_class(p, "ggplot")
})

test_that("plot.BayesMallowsSMC2 works with rho and specific items", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test with specific items
  expect_no_error(plot(mod, parameter = "rho", items = c(1, 3, 5)))
  
  p <- plot(mod, parameter = "rho", items = c(1, 3, 5))
  expect_s3_class(p, "ggplot")
})

test_that("plot.BayesMallowsSMC2 validates parameter argument", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test invalid parameter
  expect_error(plot(mod, parameter = "invalid"), "should be one of")
})

test_that("plot.BayesMallowsSMC2 validates object class", {
  # Test with non-BayesMallowsSMC2 object
  fake_obj <- list(alpha = matrix(1:10, 2, 5))
  expect_error(plot.BayesMallowsSMC2(fake_obj), "must be an object of class")
})

test_that("plot.BayesMallowsSMC2 validates items argument for rho", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test invalid item indices
  expect_error(plot(mod, parameter = "rho", items = c(1, 10)), "Invalid item indices")
  expect_error(plot(mod, parameter = "rho", items = 0), "Invalid item indices")
})
