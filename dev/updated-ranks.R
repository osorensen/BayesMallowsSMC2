devtools::load_all()
library(tidyverse)

n_items <- 10
set.seed(1)

rankings <- BayesMallows::sample_mallows(1:n_items, 10, 100, thinning = 1000)
colnames(rankings) <- paste0("item", 1:10)

complete_rankings <- tibble(timepoint = 1:100, user = 1:100) %>%
  bind_cols(rankings) %>%
  as.data.frame()

partial_rankings <- complete_rankings %>%
  mutate(across(contains("item"), ~ if_else(. > 5, NA_real_, .))) %>%
  as.data.frame()

dd1 <- partial_rankings[1:50, ]
dd2 <- partial_rankings[51:100, ]
dd2$user <- dd1$user
dd <- rbind(dd1, dd2)
dd$timepoint <- seq_len(nrow(dd))

set.seed(2)
mod <- compute_sequentially(
  dd1,
  hyperparameters = set_hyperparameters(n_items = n_items),
  smc_options = set_smc_options(n_particles = 1000, max_rejuvenation_steps = 5, verbose = TRUE,
                                trace = TRUE, trace_directory = "dev/tmp")
)

hist(mod$alpha)
