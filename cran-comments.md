## R CMD check results

0 errors | 0 warnings | 0 notes

## Test environments

* local: R 4.3.3 on Ubuntu 24.04
* GitHub Actions: R 4.1.0, 4.2.0, 4.3.0, 4.4.0 on Ubuntu, macOS, and Windows
* win-builder: development version

## Submission notes

This is a major feature release (0.3.0) for the BayesMallowsSMC2 package.

### Changes in this version

* Introduced Particle Gibbs with Backward Simulation (PG-BSi) during the rejuvenation step for mixture models via the `use_backward_simulation` option in `set_smc_options()`. This eliminates path degeneracy during conditional particle filtering, vastly improving the effective sample size and sampling efficiency.

## Previous submission

This package was initially released to CRAN as version 0.2.0. The test failures appeared after release on specific platforms (r-oldrel-macos and noLD) due to numerical differences in stochastic computations.
