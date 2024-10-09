devtools::load_all()
mod <- compute_sequentially(
  data = partial_rankings,
  hyperparameters = set_hyperparameters(n_items = 5),
  smc_options = set_smc_options(n_particles = 4, n_particle_filters = 1, max_particle_filters = 1)
)

