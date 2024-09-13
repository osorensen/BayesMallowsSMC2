set.seed(2)


m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_rankings[1:3, ]),
  smc_options = set_smc_options(n_particles = 2, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 2)
)

