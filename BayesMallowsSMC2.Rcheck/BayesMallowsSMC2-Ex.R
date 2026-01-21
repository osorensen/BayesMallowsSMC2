pkgname <- "BayesMallowsSMC2"
source(file.path(R.home("share"), "R", "examples-header.R"))
options(warn = 1)
library('BayesMallowsSMC2')

base::assign(".oldSearch", base::search(), pos = 'CheckExEnv')
base::assign(".old_wd", base::getwd(), pos = 'CheckExEnv')
cleanEx()
nameEx("compute_sequentially")
### * compute_sequentially

flush(stderr()); flush(stdout())

### Name: compute_sequentially
### Title: Compute the Bayesian Mallows model sequentially
### Aliases: compute_sequentially

### ** Examples

# Compute the model sequentially with complete rankings
mod <- compute_sequentially(
  complete_rankings,
  hyperparameters = set_hyperparameters(n_items = 5),
  smc_options = set_smc_options(n_particles = 100, n_particle_filters = 1)
)




cleanEx()
nameEx("precompute_topological_sorts")
### * precompute_topological_sorts

flush(stderr()); flush(stdout())

### Name: precompute_topological_sorts
### Title: Precompute All Topological Sorts
### Aliases: precompute_topological_sorts

### ** Examples

# Extract preferences from user 1 in the included example data.
prefs <- pairwise_preferences[
 pairwise_preferences$user == 1, c("top_item", "bottom_item"), drop = FALSE]

# Generate all topological sorts, but don't save them:
sorts <- precompute_topological_sorts(
  prefs = as.matrix(prefs),
  n_items = 5,
  save_frac = 0
)
# Number of sorts
sorts$sort_count
# Empty matrix
sorts$sort_matrix

# Generate all topological sorts and save them:
sorts <- precompute_topological_sorts(
  prefs = as.matrix(prefs),
  n_items = 5,
  save_frac = 1
)
# Number of sorts
sorts$sort_count
# Matrix with all of them
sorts$sort_matrix




### * <FOOTER>
###
cleanEx()
options(digits = 7L)
base::cat("Time elapsed: ", proc.time() - base::get("ptime", pos = 'CheckExEnv'),"\n")
grDevices::dev.off()
###
### Local variables: ***
### mode: outline-minor ***
### outline-regexp: "\\(> \\)?### [*]+" ***
### End: ***
quit('no')
