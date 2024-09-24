devtools::load_all()
m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_cluster_rankings),
  smc_options = set_smc_options(n_particles = 2000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 1)
)
ll1 <- m$log_marginal_likelihood[[nrow(complete_cluster_rankings)]]

m <- compute_sequentially(
  data = setup_rank_data(rankings = complete_cluster_rankings),
  smc_options = set_smc_options(n_particles = 2000, n_particle_filters = 1,
                                max_rejuvenation_steps = 5),
  priors = set_priors(n_clusters = 2)
)
ll2 <- m$log_marginal_likelihood[[nrow(complete_cluster_rankings)]]

new_inds <- sample(
  seq_len(ncol(m$alpha)),
  ncol(m$alpha),
  prob = m$weights,
  replace = TRUE
)
cluster_probs <- m$cluster_probs[new_inds, , ]

relab <- label.switching::stephens(cluster_probs)
alpha_smc <- t(m$alpha[, new_inds])
tau_smc <- t(m$tau[, new_inds])
rho_smc <- m$rho[, , new_inds]

for (i in 1:nrow(alpha_smc)) {
  alpha_smc[i, ] <- alpha_smc[i, relab$permutations[i, ]]
  tau_smc[i, ] <- tau_smc[i, relab$permutations[i, ]]
  rho_smc[, , i] <- rho_smc[, relab$permutations[i, ], i]
}

apply(alpha_smc, 2, mean)
apply(tau_smc, 2, mean)
apply(rho_smc, c(1, 2), mean)
