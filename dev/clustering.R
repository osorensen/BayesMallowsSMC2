library(BayesMallowsSMC2)
library(tidyverse)
library(label.switching)
#Rcpp::sourceCpp("dev/read_arma_files.cpp")

n_items <- 10
rho1 <- seq_len(n_items)
rho2 <- rev(seq_len(n_items))
alpha1 <- .3
alpha2 <- .6

rankings1 <- BayesMallows::sample_mallows(
  rho0 = rho1, alpha0 = alpha1 * n_items, n_samples = 100
)
rankings2 <- BayesMallows::sample_mallows(
  rho0 = rho2, alpha0 = alpha2 * n_items, n_samples = 100
)

colnames(rankings1) <- colnames(rankings2) <- paste0("item", seq_len(n_items))

dat <- rankings1 %>%
  as_tibble() %>%
  bind_rows(as_tibble(rankings2)) %>%
  slice_sample(prop = 1) %>%
  mutate(
    timepoint = row_number(),
    user = row_number()
  ) %>%
  select(timepoint, user, everything())

set.seed(2)
mod <- compute_sequentially(
  data = dat,
  hyperparameters = set_hyperparameters(n_items = n_items, n_clusters = 2),
  smc_options = set_smc_options(
    n_particles = 1000, n_particle_filters = 300, max_particle_filters = 300, verbose = TRUE,
    trace = FALSE)
)

perm <- stephens(mod$cluster_probabilities)

alpha <- mod$alpha
rho <- mod$rho
tau <- mod$tau

for(i in seq_len(ncol(alpha))) {
  alpha[, i] <- alpha[perm$permutations[i, ], i]
  rho[, , i] <- rho[, perm$permutations[i, ], i, drop = FALSE]
  tau[, i] <- tau[perm$permutations[i, ], i]
}

hist(alpha[1, ])
hist(alpha[2, ])


apply(tau, 1, mean)
apply(rho, c(1, 2), mean)

weighted.mean(mod$alpha[1, ], mod$importance_weights)
weighted.mean(mod$alpha[2, ], mod$importance_weights)

weighted.mean(mod$tau[1, ], mod$importance_weights)
weighted.mean(mod$tau[2, ], mod$importance_weights)
