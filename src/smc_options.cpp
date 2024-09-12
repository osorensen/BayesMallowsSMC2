#include "smc_options.h"

SMCOptions::SMCOptions(
  const std::unique_ptr<Data>& data,
  const Rcpp::List& input_smc_options
) : 
  n_particles { input_smc_options["n_particles"] },
  n_particle_filters { 
    data->type() == "CompleteRankings" ? 1 :
    input_smc_options["n_particle_filters"] },
    resampler (input_smc_options["resampler"]),
    max_rejuvenation_steps {input_smc_options["max_rejuvenation_steps"]}{}