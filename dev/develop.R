devtools::load_all()

items <- colnames(BayesMallows::sushi_rankings)
dat <- as.data.frame(BayesMallows::sushi_rankings)
colnames(dat) <- paste0("item", seq_len(ncol(dat)))
dat$user <- seq_len(nrow(dat))
dat$timepoint <- dat$user

dat <- dat[c("timepoint", "user", paste0("item", seq_along(items)))]

mod <- compute_sequentially(
  data = dat[1:250, ],
  hyperparameters = set_hyperparameters(n_items = 10, n_clusters = 5),
  smc_options = set_smc_options(n_particle_filters = 1, verbose = TRUE)
)

plot(mod$alpha_trace[1,], type = "l")
for(i in 2:5)
  lines(mod$alpha_trace[i, ], col = i)

rowMeans(mod$tau)
