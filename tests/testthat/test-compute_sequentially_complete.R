test_that("compute_sequentially works with complete data", {
  set.seed(2)
  n_items <- 10
  alpha0 <- .3
  n_users <- 100
  rankings <- BayesMallows::sample_mallows(
    rho0 = seq_len(n_items), 
    alpha0 = alpha0 * n_items,
    n_samples = n_users
  )
  colnames(rankings) <- paste0("item", seq_len(ncol(rankings)))
  
  complete_rankings <- cbind(
    data.frame(
      timepoint = seq_len(nrow(rankings)),
      user = seq_len(nrow(rankings))
    ),
    rankings
  )
  
  m <- compute_sequentially(
    data = setup_rank_data(rankings = complete_rankings),
    smc_options = set_smc_options(n_particles = 4000, n_particle_filters = 1,
                                  max_rejuvenation_steps = 3),
    priors = set_priors(n_clusters = 1)
  )
  
  wm <- weighted.mean(m$alpha, m$weights)
  expect_gt(wm, .2)
  expect_lt(wm, .4)
  
  
})
