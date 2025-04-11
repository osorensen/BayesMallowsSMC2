test_that("compute_sequentially works with preference data", {
  skip()
  dat <- subset(pairwise_preferences, user <= 20)
  sorts_dir <- paste0(tempdir(), "/sorts")
  num_topological_sorts <- as.numeric(
    lapply(split(dat, f = dat$user), function(dd) {
      precompute_topological_sorts(
        prefs = as.matrix(dd[, c("top_item", "bottom_item"), drop = FALSE]),
        n_items = 5,
        output_directory = paste0(sorts_dir, "/user", unique(dd$user)),
        save_frac = 1
      )
    })
  )

  set.seed(2)
  mod <- compute_sequentially(
    data = dat,
    hyperparameters = set_hyperparameters(n_items = 5),
    smc_options = set_smc_options(
      n_particles = 100,
      max_rejuvenation_steps = 5
      ),
    topological_sorts_directory = sorts_dir,
    num_topological_sorts = num_topological_sorts,
    file_count = num_topological_sorts
  )
})
