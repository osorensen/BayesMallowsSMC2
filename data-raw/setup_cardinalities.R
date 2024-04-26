library(tidyverse)
library(glue)

iwalk(BayesMallows:::footrule_cardinalities, function(card, n_items) {
  write_csv(
    as.data.frame(card),
    file = glue("inst/partition_function_data/footrule_{n_items}items.csv"),
    col_names = FALSE)
})

iwalk(BayesMallows:::spearman_cardinalities, function(card, n_items) {
  write_csv(
    as.data.frame(card),
    file = glue("inst/partition_function_data/spearman_{n_items}items.csv"),
    col_names = FALSE)
})

iwalk(BayesMallows:::ulam_cardinalities, function(card, n_items) {
  write_csv(
    as.data.frame(card),
    file = glue("inst/partition_function_data/ulam_{n_items}items.csv"),
    col_names = FALSE)
})
