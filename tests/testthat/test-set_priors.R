test_that("set_priors works", {
  s <- set_priors(alpha_shape = 2)
  expect_equal(s$alpha_shape, 2)
})
