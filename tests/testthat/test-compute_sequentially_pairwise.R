# test_that("compute_sequentially works with pairwise preference data", {
#   set.seed(33)
#   dat <- subset(pairwise_preferences, timepoint < 50)
#   topological_sorts_directory <- tempdir(check = TRUE)
#   num_topological_sorts <- numeric(length(unique(dat$user)))
#
#   for(i in unique(dat$user)) {
#     prefs <- as.matrix(subset(dat, subset = user == i, select = c(top_item, bottom_item)))
#
#     num_topological_sorts[[i]] <- precompute_topological_sorts(
#       prefs, 5, file.path(topological_sorts_directory, paste0("user", i)), 1000)
#   }
#
#   mod <- compute_sequentially(
#     data = dat,
#     hyperparameters = set_hyperparameters(n_items = 5),
#     smc_options = set_smc_options(n_particles = 200, resampler = "residual",
#                                   max_rejuvenation_steps = 5),
#     topological_sorts_directory = topological_sorts_directory,
#     num_topological_sorts = num_topological_sorts
#   )
#   expect_gt(mean(mod$alpha), .7)
#   expect_lt(mean(mod$alpha), 1.3)
# })
