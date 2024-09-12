test_that("set_smc_options works", {
  expect_error(
    set_smc_options(),
    "Both n_particles and n_particle_filters must be provided."
    )
  
  expect_error(
    set_smc_options(n_particles = 0, n_particle_filters = 3),
    "n_particles must be a positive integer."
  )
  
  r <- set_smc_options(n_particles = 4, n_particle_filters = 2)
  expect_equal(r$n_particles, 4)
  expect_equal(r$n_particle_filters, 2)
  
  for(resampler in c("multinomial", "residual", "stratified", "systematic")) {
    r <- set_smc_options(n_particle = 3, n_particle_filters = 2,
                         resampler = resampler)
    expect_equal(r$resampler, resampler)
  }
  
  expect_error(
    set_smc_options(n_particle = 3, n_particle_filters = 2,
                    resampler = "cauchy-schwarz"),
    "'arg' should be one of"
  )
})
