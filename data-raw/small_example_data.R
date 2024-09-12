set.seed(1)
n_items <- 5
alpha0 <- .3
n_users <- 40
rankings <- BayesMallows::sample_mallows(
  rho0 = seq_len(n_items), 
  alpha0 = alpha0 / n_items,
  n_samples = n_users
  )
colnames(rankings) <- paste0("item", seq_len(ncol(rankings)))

complete_rankings <- cbind(
  data.frame(
    timepoint = seq_len(nrow(rankings)),
    user = seq_len(nrow(rankings))
  ),
  rankings
)

usethis::use_data(complete_rankings, overwrite = TRUE)

partial_rankings <- complete_rankings
partial_rankings[, -(1:2)][partial_rankings[, -(1:2)] > 3] <- NA

usethis::use_data(partial_rankings, overwrite = TRUE)
