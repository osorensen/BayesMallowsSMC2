#pragma once
#include "options.h"

struct ProgressReporter{
  ProgressReporter(const bool verbose);
  void report_time(size_t t);
  void report_ess(double ess);
  void report_resampling();
  void report_rejuvenation(int unique_particles);
  void report_expansion(int n_particle_filters);
  void report_acceptance_rate(double acceptance_rate);

private:
  const bool verbose;
};
