test_that("compute_sequentially works", {
  m <- compute_sequentially(
    data = setup_rank_data(rankings = complete_rankings),
    smc_options = set_smc_options(n_particles = 150, n_particle_filters = 2),
    priors = set_priors(n_clusters = 1)
  )
})
