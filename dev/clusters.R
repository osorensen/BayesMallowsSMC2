devtools::load_all()

for(c in 1:3) {
  mod <- compute_sequentially(
    data = complete_rankings,
    hyperparameters = set_hyperparameters(n_items = 5, n_clusters = c),
    smc_options = set_smc_options(
      n_particles = 1000, n_particle_filters = 1, max_particle_filters = 1,
      max_rejuvenation_steps = 1
    )
  )
  print(mod$log_marginal_likelihood)
}


