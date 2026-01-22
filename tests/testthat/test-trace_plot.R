test_that("trace_plot works with alpha parameter", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      n_particle_filters = 1,
      trace = TRUE
    )
  )
  
  # Test that trace_plot function runs without error for alpha
  expect_no_error(trace_plot(mod, parameter = "alpha"))
  expect_no_error(trace_plot(mod))  # alpha is default
  
  # Check that it returns a ggplot object
  p <- trace_plot(mod, parameter = "alpha")
  expect_s3_class(p, "ggplot")
})

test_that("trace_plot works with tau parameter", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      n_particle_filters = 1,
      trace = TRUE
    )
  )
  
  # Test that trace_plot function runs without error for tau
  expect_no_error(trace_plot(mod, parameter = "tau"))
  
  # Check that it returns a ggplot object
  p <- trace_plot(mod, parameter = "tau")
  expect_s3_class(p, "ggplot")
})

test_that("trace_plot validates parameter argument", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      n_particle_filters = 1,
      trace = TRUE
    )
  )
  
  # Test invalid parameter
  expect_error(trace_plot(mod, parameter = "invalid"), "should be one of")
})

test_that("trace_plot validates object class", {
  # Test with non-BayesMallowsSMC2 object
  fake_obj <- list(alpha_traces = list(matrix(1:10, 2, 5)))
  expect_error(trace_plot(fake_obj), "must be an object of class")
})

test_that("trace_plot requires trace = TRUE", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      n_particle_filters = 1,
      trace = FALSE  # trace disabled
    )
  )
  
  # Test that it gives a helpful error message
  expect_error(trace_plot(mod), "Trace data not found")
  expect_error(trace_plot(mod), "trace = TRUE")
})

test_that("trace_plot works with mixture models", {
  # Create mixture data (using partial rankings as a proxy)
  set.seed(456)
  mod <- compute_sequentially(
    partial_rankings[1:20, ],
    hyperparameters = set_hyperparameters(n_items = 5, n_clusters = 2),
    smc_options = set_smc_options(
      n_particles = 50,
      n_particle_filters = 1,
      trace = TRUE
    )
  )
  
  # Test that trace_plot works with multiple clusters
  expect_no_error(p <- trace_plot(mod, parameter = "alpha"))
  expect_s3_class(p, "ggplot")
  
  expect_no_error(p <- trace_plot(mod, parameter = "tau"))
  expect_s3_class(p, "ggplot")
})
