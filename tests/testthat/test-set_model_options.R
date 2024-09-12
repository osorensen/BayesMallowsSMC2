test_that("set_model_options works", {
  m <- set_model_options()
  expect_equal(m$metric, "footrule")
  
  for(x in c("footrule", "spearman", "kendall", "cayley", "ulam", "hamming")) {
    m <- set_model_options(metric = x)
    expect_equal(m$metric, x)
  }
  
  expect_error(
    set_model_options(metric = "euclidean"),
    "'arg' should be one of"
    )
})
