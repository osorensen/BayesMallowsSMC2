library(tidyverse)

n_items <- 5
set.seed(1)

rankings <- BayesMallows::sample_mallows(1:5, 5, 100, thinning = 1000)
colnames(rankings) <- paste0("item", 1:5)

complete_rankings <- tibble(timepoint = 1:100, user = 1:100) %>%
  bind_cols(rankings) %>%
  as.data.frame()

usethis::use_data(complete_rankings, overwrite = TRUE)

partial_rankings <- complete_rankings %>%
  mutate(across(contains("item"), ~ if_else(runif(100) > .8, NA_real_, .))) %>%
  as.data.frame()

usethis::use_data(partial_rankings, overwrite = TRUE)

possible_pairs <- combn(1:5, 2, simplify = FALSE)
pairwise_preferences <- complete_rankings %>%
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

usethis::use_data(pairwise_preferences, overwrite = TRUE)
