test_that("compute_sequentially works with preference data", {
  dat <- subset(pairwise_preferences, user <= 20)
  topological_sorts <- split(dat, f =~ timepoint) |>
    lapply(split, f =~ user) |>
    lapply(function(x) {
      lapply(x, function(y) {
        precompute_topological_sorts(
          prefs = as.matrix(y[, c("top_item", "bottom_item"), drop = FALSE]),
          n_items = 5,
          save_frac = 1
        )
      })
    })

  set.seed(2)
  mod <- compute_sequentially(
    data = dat,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      max_rejuvenation_steps = 5
      ),
    topological_sorts = topological_sorts
  )

  expect_gt(mean(mod$alpha), 1.03)
  expect_lt(mean(mod$alpha), 1.06)
})
