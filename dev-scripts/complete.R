
m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_cluster_rankings),
  smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 1)
)


logmax <- max(m$log_marginal_likelihood)
logmax + sum(exp(m$log_marginal_likelihood - logmax))

m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_cluster_rankings),
  smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 2)
)


logmax <- max(m$log_marginal_likelihood)
logmax + sum(exp(m$log_marginal_likelihood - logmax))

m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_cluster_rankings),
  smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 3)
)


logmax <- max(m$log_marginal_likelihood)
logmax + sum(exp(m$log_marginal_likelihood - logmax))

m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_rankings),
  smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 1)
)


logmax <- max(m$log_marginal_likelihood)
logmax + sum(exp(m$log_marginal_likelihood - logmax))

m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_rankings),
  smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 2)
)


logmax <- max(m$log_marginal_likelihood)
logmax + sum(exp(m$log_marginal_likelihood - logmax))

m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_rankings),
  smc_options = set_smc_options(n_particles = 1000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 3)
)


logmax <- max(m$log_marginal_likelihood)
logmax + sum(exp(m$log_marginal_likelihood - logmax))