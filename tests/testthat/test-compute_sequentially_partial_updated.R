# dd1 <- partial_rankings[1:50, ]
# #dd1[, -(1:2)][dd1[, -(1:2)] > 2] <- NA
# dd2 <- partial_rankings[51:100, ]
# #dd2[, -(1:2)][dd2[, -(1:2)] > 3] <- NA
# dd2$user <- dd1$user
# dd <- rbind(dd1, dd2)
# dd$timepoint <- seq_len(nrow(dd))
#
# test_that("compute_sequentially works with new and updated users", {
#   set.seed(2)
#   mod <- compute_sequentially(
#     dd,
#     hyperparameters = set_hyperparameters(n_items = 5),
#     smc_options = set_smc_options(
#       n_particles = 100, n_particle_filters = 1, verbose = T,
#       max_particle_filters = 20, max_rejuvenation_steps = 1,
#       trace = FALSE, trace_directory = "tmp")
#   )
#   hist(mod$alpha)
#   alpha_hat <- weighted.mean(x = as.numeric(mod$alpha), w = mod$importance_weights)
#   expect_gt(alpha_hat, .94)
#   expect_lt(alpha_hat, 1.1)
# })
