Rcpp::sourceCpp("dev/rmultinom.cpp")

prob <- runif(200)
prob <- prob/sum(prob) # standardize the probabilities
size <- 500

set.seed(10)
sim_r <- rmultinom(1, size, prob)
set.seed(10)
sim_rcpp <- resample_counts(size, prob)


all.equal(sim_r, sim_rcpp)



count_between_intervals(
  u = c(.05, .08, .11, .9),
  cumprob = c(.1, .15, .2, 1)
  )
