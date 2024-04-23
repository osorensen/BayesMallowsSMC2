library(tidyverse)
library(mvtnorm)
n_items <- 5
set.seed(1)
complete_rankings <- tibble(
  timepoint = 1:100,
  user = 1:100
) %>%
  pmap_dfr(function(timepoint, user) {
    tibble(
      timepoint = timepoint,
      user = user,
      item = seq_len(n_items),
      ranking = order(rmvnorm(1, mean = seq(from = 0, to = 1, length.out = n_items)))
    )
  }) %>%
  pivot_wider(names_from = item, values_from = ranking, names_prefix = "item") %>%
  as.data.frame()

usethis::use_data(complete_rankings, overwrite = TRUE)

partial_rankings <- complete_rankings %>%
  mutate(across(contains("item"), ~ if_else(runif(100) > .8, NA_real_, .))) %>%
  as.data.frame()

usethis::use_data(partial_rankings, overwrite = TRUE)
