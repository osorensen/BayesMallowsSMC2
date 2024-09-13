devtools::load_all()

tmp <- partial_rankings[1:6, ]
# tmp$user[4:6] <- tmp$user[1:3]  
# tmp$timepoint <- c(1,1,2,2,3,3)

m <- compute_sequentially(
  data = setup_rank_data(tmp),
  smc_options = set_smc_options(n_particles = 2, n_particle_filters = 2)
)
