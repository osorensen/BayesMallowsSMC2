library(tidyverse)
library(mvtnorm)

set.seed(123)
component1 <- BayesMallows::sample_mallows(
  rho0 = 1:5, alpha0 = 10, n_samples = 200, thinning = 300
)

component2 <- BayesMallows::sample_mallows(
  rho0 = 5:1, alpha0 = 5, n_samples = 200, thinning = 300
)

rankings <- rbind(component1, component2)
colnames(rankings) <- paste0("item", 1:5)
mixtures <- as_tibble(rankings) %>%
  slice_sample(prop = 1) %>%
  mutate(
    timepoint = row_number(),
    user = row_number()
  ) %>%
  select(timepoint, user, everything()) %>%
  as.data.frame()

usethis::use_data(mixtures, overwrite = TRUE)
