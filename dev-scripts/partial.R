devtools::load_all()

n_items <- 5
alpha0 <- 1
n_users <- 50
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

partial_rankings <- complete_rankings
partial_rankings[, -(1:2)][partial_rankings[, -(1:2)] > 3] <- NA

m <- compute_sequentially(
  data = setup_rank_data(rankings = partial_rankings),
  smc_options = set_smc_options(n_particles = 100, n_particle_filters = 10,
                                max_rejuvenation_steps = 5, verbose = TRUE)
)

apply(m$rho, c(1, 2), mean)

wm <- weighted.mean(as.numeric(m$alpha), m$weights)
expect_gt(wm, .2)
expect_lt(wm, .4)

m$log_marginal_likelihood
hist(m$alpha)
hist(m$tau)