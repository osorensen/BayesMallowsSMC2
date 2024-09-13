set.seed(1)
n_items <- 10
alpha0 <- c(.2, .4)
n_users <- c(200, 200)

rankings1 <- BayesMallows::sample_mallows(
  rho0 = seq_len(n_items), 
  alpha0 = alpha0[[1]] * n_items,
  n_samples = n_users[[1]]
)
rankings2 <- BayesMallows::sample_mallows(
  rho0 = rev(seq_len(n_items)), 
  alpha0 = alpha0[[2]] * n_items,
  n_samples = n_users[[2]]
)

rankings <- rbind(rankings1, rankings2)
colnames(rankings) <- paste0("item", seq_len(ncol(rankings)))

complete_cluster_rankings <- cbind(
  data.frame(
    timepoint = seq_len(nrow(rankings)),
    user = seq_len(nrow(rankings))
  ),
  rankings[sample(nrow(rankings)), ]
)
