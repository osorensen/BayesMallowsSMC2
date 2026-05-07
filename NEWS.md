# BayesMallowsSMC2 version 0.3.0

## Major changes

* Introduced `use_backward_simulation` argument in `set_smc_options()` to toggle Particle Gibbs with Backward Simulation (PG-BSi) during the rejuvenation step for mixture models. This vastly improves the effective sample size per second for clustering models.

# BayesMallowsSMC2 version 0.2.1

## Bug fixes

* Adjusted test tolerance in `test-compute_sequentially_partial.R` to account for platform-specific numerical differences that caused test failures on r-oldrel-macos and noLD platforms.

# BayesMallowsSMC2 version 0.2.0

Initial release.
