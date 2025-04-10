test_that("Mixture models work", {
  set.seed(2)
  mod <- compute_sequentially(
    mixtures[1:50, ],
    hyperparameters = set_hyperparameters(n_items = 5, n_clusters = 2),
    smc_options = set_smc_options(
      n_particles = 100, n_particle_filters = 5, max_particle_filters = 5)
  )

  perm <- label.switching::stephens(mod$cluster_probabilities)

  alpha <- mod$alpha
  rho <- mod$rho
  tau <- mod$tau

  for(i in seq_len(ncol(alpha))) {
    alpha[, i] <- alpha[perm$permutations[i, ], i]
    rho[, , i] <- rho[, perm$permutations[i, ], i, drop = FALSE]
    tau[, i] <- tau[perm$permutations[i, ], i]
  }

  expect_gt(weighted.mean(alpha[1, ], mod$importance_weights), 1)
  expect_lt(weighted.mean(alpha[1, ], mod$importance_weights), 1.1)
  expect_gt(weighted.mean(alpha[2, ], mod$importance_weights), 2)
  expect_lt(weighted.mean(alpha[2, ], mod$importance_weights), 2.5)

  expect_gt(weighted.mean(tau[1, ], mod$importance_weights), .4)
  expect_lt(weighted.mean(tau[1, ], mod$importance_weights), .6)
  expect_gt(weighted.mean(tau[2, ], mod$importance_weights), .4)
  expect_lt(weighted.mean(tau[2, ], mod$importance_weights), .6)
})
