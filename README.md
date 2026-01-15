
<!-- README.md is generated from README.Rmd. Please edit that file -->

# BayesMallowsSMC2

<!-- badges: start -->

[![R-CMD-check](https://github.com/osorensen/BayesMallowsSMC2/actions/workflows/R-CMD-check.yaml/badge.svg)](https://github.com/osorensen/BayesMallowsSMC2/actions/workflows/R-CMD-check.yaml)
<!-- badges: end -->

BayesMallowsSMC2 provides functions for performing sequential inference
in the Bayesian Mallows model using the SMC$^{2}$ algorithm. The package
implements the methods described in Sørensen, Frigessi, and Scheel
(2025).

The Bayesian Mallows model is a probabilistic framework for analyzing
ranking data, and this package extends it to handle sequential learning
scenarios where rankings arrive over time. The SMC$^{2}$ (Sequential
Monte Carlo squared) algorithm enables efficient Bayesian inference by
combining particle filtering with MCMC methods.

## Installation

You can install the development version of BayesMallowsSMC2 from
[GitHub](https://github.com/) with:

``` r
# install.packages("devtools")
devtools::install_github("osorensen/BayesMallowsSMC2")
```

## Usage

This package implements sequential Bayesian inference for ranking data
using the Mallows model. The main function is `compute_sequentially()`,
which performs SMC$^{2}$ inference as rankings arrive over time.

``` r
library(BayesMallowsSMC2)

# Example usage (see vignettes for detailed examples)
# result <- compute_sequentially(data, hyperparameters, smc_options)
```

For detailed examples and reproducible code, see the OSF repository at
<https://osf.io/pquk4/>.

## References

<div id="refs" class="references csl-bib-body hanging-indent"
entry-spacing="0">

<div id="ref-sorensen2025sequential" class="csl-entry">

Sørensen, Øystein, Arnoldo Frigessi, and Ida Scheel. 2025. “Sequential
Rank and Preference Learning with the Bayesian Mallows Model.” *Bayesian
Analysis*. <https://doi.org/10.1214/25-BA1564>.

</div>

</div>
