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

create_partial <- function(complete_rankings, missing_prob) {
  complete_rankings %>%
    mutate(across(contains("item"), ~ if_else(runif(nrow(complete_rankings)) > 1 - missing_prob, NA_real_, .))) %>%
    as.data.frame()
}

partial_rankings_updated <- rbind(
  create_partial(complete_rankings, .8),
  create_partial(complete_rankings, .6),
  create_partial(complete_rankings, .4),
  create_partial(complete_rankings, .2)
) %>%
  mutate(timepoint = row_number())

usethis::use_data(partial_rankings_updated, overwrite = TRUE)
