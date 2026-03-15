## R CMD check results

0 errors | 0 warnings | 0 notes

## Test environments

* local: R 4.3.3 on Ubuntu 24.04
* GitHub Actions: R 4.1.0, 4.2.0, 4.3.0, 4.4.0 on Ubuntu, macOS, and Windows
* win-builder: development version

## Submission notes

This release introduces major algorithmic improvements for the SMC2 rejuvenation step.

### Changes in this version

* Implemented Conditional Particle Filter with Independent Backward Simulation (CPF-IBS) for the rejuvenation step. This resolves path degeneracy in latent variables during Particle Gibbs and improves mixing. It is available via the `backward_sampling` argument in `set_smc_options()`.
* Retained numerical tolerance fixes from 0.2.1 that accommodated platform-specific variations on r-oldrel-macos and noLD.
