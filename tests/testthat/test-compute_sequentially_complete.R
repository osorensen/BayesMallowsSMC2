test_that("compute_sequentially works with complete data", {
  set.seed(2)
  
  m <- compute_sequentially(
    data = setup_rank_data(rankings = complete_rankings),
    smc_options = set_smc_options(n_particles = 4000, n_particle_filters = 1,
                                  max_rejuvenation_steps = 5),
    model_options = set_model_options(n_clusters = 1)
  )
  
  wm <- weighted.mean(as.numeric(m$alpha), m$weights)
  expect_gt(wm, .28)
  expect_lt(wm, .36)
  
  new_inds <- sample(length(m$alpha), length(m$alpha), prob = m$weights, replace = TRUE)
  expect_gt(mean(m$alpha[new_inds]), .28)
  expect_lt(mean(m$alpha[new_inds]), .36)
  
  expect_lte(
    max(abs(order(apply(m$rho[, new_inds], 1, mean)) - 1:10)),
    1)
  
  hist <- table(apply(m$rho[, new_inds], 2, paste, collapse = ","))
  expect_gt(length(hist), 10)  
  expect_lt(length(hist), 2000)  
})
