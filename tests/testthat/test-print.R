test_that("print method works for BayesMallowsSMC2 objects", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test that print method runs without error
  expect_error(print(mod), NA)
  
  # Test that print returns the object invisibly
  expect_identical(print(mod), mod)
  
  # Capture output and verify it contains expected content
  output <- capture.output(print(mod))
  expect_true(any(grepl("BayesMallowsSMC2 Model", output)))
  expect_true(any(grepl("Number of particles:", output)))
  expect_true(any(grepl("Number of timepoints:", output)))
  expect_true(any(grepl("Number of items:", output)))
  expect_true(any(grepl("Number of clusters:", output)))
  expect_true(any(grepl("Log marginal likelihood:", output)))
  expect_true(any(grepl("Final ESS:", output)))
  expect_true(any(grepl("Resampling events:", output)))
})

test_that("print method works with partial rankings", {
  set.seed(456)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 50, n_particle_filters = 1)
  )
  
  # Test that print method runs without error
  expect_error(print(mod), NA)
  
  # Capture output and verify it contains expected content
  output <- capture.output(print(mod))
  expect_true(any(grepl("BayesMallowsSMC2 Model", output)))
})
