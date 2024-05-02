library(tidyverse)
library(mvtnorm)
n_items <- 5
set.seed(1)
complete_rankings_mixtures <- tibble(
  timepoint = 1:1000,
  user = 1:1000
) %>%
  pmap_dfr(function(timepoint, user) {
    decreasing <- sample(c(TRUE, FALSE), 1)
    tibble(
      timepoint = timepoint,
      user = user,
      item = seq_len(n_items),
      ranking = order(rmvnorm(1, mean = seq(from = 0, to = 1, length.out = n_items)), decreasing = !!decreasing)
    )
  }) %>%
  pivot_wider(names_from = item, values_from = ranking, names_prefix = "item") %>%
  as.data.frame()

usethis::use_data(complete_rankings_mixtures, overwrite = TRUE)

partial_rankings_mixtures <- complete_rankings_mixtures %>%
  mutate(across(contains("item"), ~ if_else(runif(1000) > .8, NA_real_, .))) %>%
  as.data.frame()

usethis::use_data(partial_rankings_mixtures, overwrite = TRUE)

possible_pairs <- combn(1:5, 2, simplify = FALSE)
pairwise_preferences_mixtures <- complete_rankings_mixtures %>%
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

usethis::use_data(pairwise_preferences_mixtures, overwrite = TRUE)
