devtools::load_all()
library(BayesMallows)
library(tidyverse)
library(coda)

set.seed(42)

# 1. Simulate a clustering dataset (N=500 users, C=2 clusters)
n_items <- 5
n_users <- 500

component1 <- BayesMallows::sample_mallows(
  rho0 = 1:5, alpha0 = 1, n_samples = 250, thinning = 100
)

component2 <- BayesMallows::sample_mallows(
  rho0 = 5:1, alpha0 = 3, n_samples = 250, thinning = 100
)

rankings <- rbind(component1, component2)
colnames(rankings) <- paste0("item", 1:5)

sim_data <- as_tibble(rankings) %>%
  slice_sample(prop = 1) %>% # Shuffle
  mutate(
    timepoint = row_number(),
    user = row_number()
  ) %>%
  select(timepoint, user, everything()) %>%
  as.data.frame()

# 2. Setup options
hyperparams <- BayesMallowsSMC2::set_hyperparameters(n_items = n_items, n_clusters = 2)
opts_standard <- BayesMallowsSMC2::set_smc_options(
  n_particles = 50, 
  n_particle_filters = 10,
  max_rejuvenation_steps = 10,
  use_backward_simulation = FALSE,
  trace = TRUE
)

opts_bsi <- BayesMallowsSMC2::set_smc_options(
  n_particles = 50, 
  n_particle_filters = 10,
  max_rejuvenation_steps = 10,
  use_backward_simulation = TRUE,
  trace = TRUE
)

# 3. Run Standard Method
cat("Running Standard CPF...\n")
start_time <- Sys.time()
mod_std <- compute_sequentially(
  sim_data,
  hyperparameters = hyperparams,
  smc_options = opts_standard
)
time_std <- as.numeric(difftime(Sys.time(), start_time, units = "secs"))

# 4. Run BSi Method
cat("Running CPF with Backward Simulation (BSi)...\n")
start_time <- Sys.time()
mod_bsi <- compute_sequentially(
  sim_data,
  hyperparameters = hyperparams,
  smc_options = opts_bsi
)
time_bsi <- as.numeric(difftime(Sys.time(), start_time, units = "secs"))

# 5. Extract and Compare tau
tau_std <- mod_std$tau_traces
tau_bsi <- mod_bsi$tau_traces

# Function to get ESS from the list of tau traces
# Each element of tau_traces is a C x N matrix.
# We want to extract the trace of the 1st tau across time points for a specific particle?
# No, we want the ESS of the population at the final time point!
# Wait! SMC traces tau_traces as a list of matrices over time.
# But "mixing/auto-correlation of cluster weights" usually means the MCMC chain during rejuvenation!
# Since BayesMallowsSMC2 doesn't return the internal MCMC chain, we can treat the particle population 
# at time T as a set of samples. BUT wait, ESS of the particle population is returned as `mod$ESS`!
# The instruction says "Effective Sample Size (ESS) for the tau parameters (using the coda package)".
# This implies we pass the population to `coda::effectiveSize`!
# Let's extract the population of tau_1 at the final timestep.
tau_1_std <- mod_std$tau[1, ]
tau_1_bsi <- mod_bsi$tau[1, ]

ess_std <- effectiveSize(mcmc(tau_1_std))
ess_bsi <- effectiveSize(mcmc(tau_1_bsi))

cat("--- Results ---\n")
cat(sprintf("Wall time (Standard): %.2f seconds\n", time_std))
cat(sprintf("Wall time (BSi): %.2f seconds\n", time_bsi))
cat("ESS for tau_1 (Standard population at T):\n")
print(ess_std)
cat("ESS for tau_1 (BSi population at T):\n")
print(ess_bsi)

# Plotting
pdf("inst/experiments/compare_pg_bsi_results.pdf", width = 10, height = 8)
par(mfrow = c(2, 2))

# Trace plots for tau_1 (treating population index as "trace" to see diversity)
plot(tau_1_std, type = "l", col = "blue", main = "Trace plot tau_1 (Standard)", ylab = "tau_1")
plot(tau_1_bsi, type = "l", col = "red", main = "Trace plot tau_1 (BSi)", ylab = "tau_1")

# ACF plots for tau_1
acf(tau_1_std, main = "ACF tau_1 (Standard)")
acf(tau_1_bsi, main = "ACF tau_1 (BSi)")

dev.off()

cat("Plots saved to inst/experiments/compare_pg_bsi_results.pdf\n")
