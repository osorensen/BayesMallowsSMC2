library(tidyverse)
library(mvtnorm)
n_items <- 5
set.seed(1)
complete_rankings_heterogeneous <- tibble(
  timepoint = sample(1:100, 200, replace = TRUE)
) %>%
  arrange(timepoint) %>%
  mutate(
    timepoint = as.integer(as.factor(timepoint)),
    user = row_number()
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

usethis::use_data(complete_rankings_heterogeneous, overwrite = TRUE)

partial_rankings_heterogeneous <- complete_rankings_heterogeneous %>%
  mutate(across(contains("item"), ~ if_else(runif(nrow(complete_rankings_heterogeneous)) > .8, NA_real_, .))) %>%
  as.data.frame()

usethis::use_data(partial_rankings_heterogeneous, overwrite = TRUE)

possible_pairs <- combn(1:5, 2, simplify = FALSE)
pairwise_preferences_heterogeneous <- complete_rankings_heterogeneous %>%
  pivot_longer(cols = starts_with("item")) %>%
  mutate(name = str_extract(name, "[:digit:]+$")) %>%
  nest_by(timepoint, user, .keep = TRUE) %>%
  pmap_dfr(function(timepoint, user, data) {
    chosen_pairs <- possible_pairs[sample(length(possible_pairs), 4)]
    map_dfr(chosen_pairs, function(p) {
      data %>%
        slice(p) %>%
        arrange(value) %>%
        mutate(value = c("top_item", "bottom_item")) %>%
        pivot_wider(names_from = value, values_from = name) %>%
        mutate(across(contains("item"), as.integer))
    })
  }) %>%
  as.data.frame()

usethis::use_data(pairwise_preferences_heterogeneous, overwrite = TRUE)
