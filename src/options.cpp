#include "options.h"

Options::Options(const Rcpp::List& input_options) :
  metric ( input_options["metric"] ),
  n_particles {input_options["n_particles"]},
  n_particle_filters {input_options["n_particle_filters"]}{}
