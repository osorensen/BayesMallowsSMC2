test_that("compute_sequentially works with complete data", {
  set.seed(2)
  
  m <- compute_sequentially(
    data = setup_rank_data(rankings = complete_rankings),
    smc_options = set_smc_options(n_particles = 2000, n_particle_filters = 1,
                                  max_rejuvenation_steps = 5),
    priors = set_priors(n_clusters = 1)
  )
  
  wm <- weighted.mean(as.numeric(m$alpha), m$weights)
  expect_gt(wm, .28)
  expect_lt(wm, .36)
  
})
