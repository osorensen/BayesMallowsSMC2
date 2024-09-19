test_that("compute_sequentially works with partial data", {
  # set.seed(2)
  # 
  # m <- compute_sequentially(
  #   data = setup_rank_data(rankings = partial_rankings),
  #   smc_options = set_smc_options(n_particles = 300, n_particle_filters = 100,
  #                                 max_rejuvenation_steps = 5),
  #   priors = set_priors(n_clusters = 1)
  # )
  # 
  # wm <- weighted.mean(as.numeric(m$alpha), m$weights)
  # expect_gt(wm, .2)
  # expect_lt(wm, .4)
  
})
