test_that("compute_sequentially works with preference data", {
  dat <- subset(pairwise_preferences, user <= 24)
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

  expect_gt(mean(mod$alpha), 1)
  expect_lt(mean(mod$alpha), 1.02)
})

test_that("compute_sequentially works with preference data and tracing", {
  dat <- subset(pairwise_preferences, user <= 3)
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
      max_rejuvenation_steps = 5,
      trace = TRUE, trace_latent = FALSE
    ),
    topological_sorts = topological_sorts
  )

  expect_equal(length(mod$alpha_traces), 3)
  expect_equal(length(mod$alpha_traces[[2]]), 100)
  expect_gt(mod$alpha_traces[[2]][[3]], .24)
  expect_lt(mod$alpha_traces[[2]][[3]], .25)

  set.seed(3)
  mod <- compute_sequentially(
    data = dat,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      max_rejuvenation_steps = 5,
      trace = TRUE, trace_latent = TRUE
    ),
    topological_sorts = topological_sorts
  )

  expect_equal(length(mod$alpha_traces), 3)
  expect_equal(length(mod$alpha_traces[[2]]), 100)
  expect_gt(mod$alpha_traces[[2]][[3]], .04)
  expect_lt(mod$alpha_traces[[2]][[3]], .05)

  expect_equal(length(mod$latent_rankings_traces), 3)
  expect_equal(length(mod$latent_rankings_traces[[2]]), 100)
  expect_equal(
    mod$latent_rankings_traces[[2]][[3]],
    c(1, 3, 5, 4, 2, 2, 1, 4, 3, 5)
  )
})
