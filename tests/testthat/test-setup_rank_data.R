test_that("setup_rank_data works", {
  r <- setup_rank_data(complete_rankings)
  expect_equal(r$type, "complete_rankings")
  expect_equal(r$n_items, 10)
  
  expect_error(
    setup_rank_data(complete_rankings[, 1:2]),
    "No data, nothing to do"
    )
  
  expect_error(
    setup_rank_data(complete_rankings[FALSE, ]),
    "No data, nothing to do"
  )
  
  expect_error(
    setup_rank_data(),
    "Either rankings or preferences must be provided"
  )
  
  r <- setup_rank_data(partial_rankings)
  expect_equal(r$type, "partial_rankings")
  expect_equal(r$n_items, 10)
})

