#include <RcppArmadillo.h>
#include "progress_reporter.h"

ProgressReporter::ProgressReporter(const bool verbose) :
  verbose { verbose } {}

void ProgressReporter::report_time(size_t t) {
  if(verbose){
    Rcpp::Rcout << "t = " << t << std::endl;
  }
}

void ProgressReporter::report_ess(double ess) {
  if(verbose) {
    Rcpp::Rcout << "effective sample size = " << ess << std::endl;
  }
}

void ProgressReporter::report_resampling() {
  if(verbose) {
    Rcpp::Rcout << "starting resampling" << std::endl;
  }
}

void ProgressReporter::report_rejuvenation(int unique_particles) {
  if(verbose) {
    Rcpp::Rcout << unique_particles << " unique particles after rejuvenation" << std::endl;
  }
}

void ProgressReporter::report_expansion(int n_particle_filters) {
  if(verbose) {
    Rcpp::Rcout << n_particle_filters << " particle filters after doubling" << std::endl;
  }
}
