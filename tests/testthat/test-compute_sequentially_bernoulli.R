test_that("Bernoulli error model works with non-transitive preferences", {
  # Simulate non-transitive pairwise preferences: A > B, B > C, C > A
  # Items: 1 = A, 2 = B, 3 = C
  preferences <- matrix(c(
    1, 2,
    2, 3,
    3, 1
  ), ncol = 2, byrow = TRUE)
  
  df <- data.frame(
    timepoint = 1,
    user = 1,
    top_item = preferences[, 1],
    bottom_item = preferences[, 2]
  )
  
  # When using pairwise preferences, we need precomputed topological sorts,
  # but for bernoulli error model, they are technically bypassed.
  # However, the R code still expects them.
  top_sorts <- list("1" = list("1" = list(sort_count = 0, sort_matrix = matrix(numeric(0), nrow = 0, ncol = 0))))
  
  # Run compute_sequentially
  mod <- compute_sequentially(
    df,
    topological_sorts = top_sorts,
    smc_options = set_smc_options(
      n_particles = 100, 
      n_particle_filters = 5,
      error_model = "bernoulli",
      use_backward_simulation = TRUE
    ),
    hyperparameters = set_hyperparameters(n_items = 3)
  )
  
  # Assertions
  expect_true(!is.null(mod$epsilon))
  expect_true(mean(mod$epsilon) > 0)
  expect_true(mean(mod$epsilon) < 0.5)
})
