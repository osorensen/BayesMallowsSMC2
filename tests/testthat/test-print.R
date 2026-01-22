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

test_that("summary method works for BayesMallowsSMC2 objects", {
  set.seed(123)
  mod <- compute_sequentially(
    complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
  )
  
  # Test that summary method runs without error
  expect_error(summary(mod), NA)
  
  # Test that summary returns an object of the correct class
  summ <- summary(mod)
  expect_s3_class(summ, "summary.BayesMallowsSMC2")
  
  # Test that summary object contains expected fields
  expect_true("n_particles" %in% names(summ))
  expect_true("n_timepoints" %in% names(summ))
  expect_true("n_items" %in% names(summ))
  expect_true("n_clusters" %in% names(summ))
  expect_true("log_marginal_likelihood" %in% names(summ))
  expect_true("final_ess" %in% names(summ))
  expect_true("n_resampling_events" %in% names(summ))
  expect_true("alpha_mean" %in% names(summ))
  expect_true("alpha_sd" %in% names(summ))
  
  # Test that alpha statistics are numeric
  expect_type(summ$alpha_mean, "double")
  expect_type(summ$alpha_sd, "double")
  
  # Test that alpha statistics have correct length (equal to number of clusters)
  expect_equal(length(summ$alpha_mean), summ$n_clusters)
  expect_equal(length(summ$alpha_sd), summ$n_clusters)
  
  # Test that print method for summary works
  expect_error(print(summ), NA)
  
  # Capture output and verify it contains expected content
  output <- capture.output(print(summ))
  expect_true(any(grepl("BayesMallowsSMC2 Model Summary", output)))
  expect_true(any(grepl("Number of particles:", output)))
  expect_true(any(grepl("Number of timepoints:", output)))
  expect_true(any(grepl("Number of items:", output)))
  expect_true(any(grepl("Number of clusters:", output)))
  expect_true(any(grepl("Log marginal likelihood:", output)))
  expect_true(any(grepl("Final ESS:", output)))
  expect_true(any(grepl("Resampling events:", output)))
  expect_true(any(grepl("Posterior Statistics for Alpha:", output)))
  expect_true(any(grepl("Mean =", output)))
  expect_true(any(grepl("SD =", output)))
})

test_that("summary method works with partial rankings", {
  set.seed(456)
  mod <- compute_sequentially(
    partial_rankings,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(n_particles = 50, n_particle_filters = 1)
  )
  
  # Test that summary method runs without error
  expect_error(summary(mod), NA)
  
  # Test summary object has correct structure
  summ <- summary(mod)
  expect_s3_class(summ, "summary.BayesMallowsSMC2")
  
  # Capture output and verify it contains expected content
  output <- capture.output(print(summ))
  expect_true(any(grepl("BayesMallowsSMC2 Model Summary", output)))
  expect_true(any(grepl("Posterior Statistics for Alpha:", output)))
})
