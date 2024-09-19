devtools::load_all()

m <- compute_sequentially(
  data = setup_rank_data(partial_rankings[1:30, ]),
  smc_options = set_smc_options(n_particles = 100, n_particle_filters = 50)
)

new_inds <- sample(length(m$weights), length(m$weights), replace = TRUE, prob = m$weights)

