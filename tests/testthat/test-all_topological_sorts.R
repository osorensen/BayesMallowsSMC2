test_that("precompute_topological_sorts works", {
  prefs <- pairwise_preferences[
    pairwise_preferences$user == 1, c("top_item", "bottom_item"), drop = FALSE]

  sorts <- precompute_topological_sorts(
    prefs = as.matrix(prefs),
    n_items = 5,
    save_frac = 0
  )

  expect_equal(dim(sorts$sort_matrix), c(0, 0))

  set.seed(1)
  sorts <- precompute_topological_sorts(
    prefs = as.matrix(prefs),
    n_items = 5,
    save_frac = .5
  )

  expect_equal(
    sorts$sort_matrix,
    structure(c(5L, 1L, 2L, 3L, 4L, 5L, 1L, 2L, 4L, 3L, 5L, 1L, 4L,
                2L, 3L), dim = c(5L, 3L))
  )

  expect_equal(sorts$sort_count, 8L)
})
