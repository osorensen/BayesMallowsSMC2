# BayesMallowsSMC2 version 0.3.0

## New features

* Implemented Conditional Particle Filter with Independent Backward Simulation (CPF-IBS) to mitigate path degeneracy during the rejuvenation step. This can be enabled via `backward_sampling = TRUE` in `set_smc_options()`.

# BayesMallowsSMC2 version 0.2.1

## Bug fixes

* Adjusted test tolerance in `test-compute_sequentially_partial.R` to account for platform-specific numerical differences that caused test failures on r-oldrel-macos and noLD platforms.

# BayesMallowsSMC2 version 0.2.0

Initial release.
