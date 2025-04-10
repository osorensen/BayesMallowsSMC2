#include <sys/stat.h>
#include <string>
#include <cstdlib>
#include <sstream>
#include "parameter_tracer.h"
using namespace arma;

ParameterTracer::ParameterTracer(bool trace, bool trace_latent, const std::string& trace_directory)
  : trace { trace }, trace_latent { trace_latent }, trace_directory { trace_directory } {
  if(trace) {
#ifdef _WIN32
    // On Windows, mkdir takes only one argument
    int status = mkdir(trace_directory.c_str());
#else
    // On POSIX systems, mkdir takes two arguments
    int status = mkdir(trace_directory.c_str(), 0777);
#endif
    if (status != 0) {
      Rcpp::stop("Error creating trace directory.");
    }
  }
}

void ParameterTracer::update_trace(const std::vector<Particle>& pvec, int t) {
  if(trace) {
    mat alpha(pvec[0].parameters.alpha.n_rows, pvec.size());
    for(size_t i{}; i < pvec.size(); i++) {
      alpha.col(i) = pvec[i].parameters.alpha;
    }

    std::ostringstream filename_stream;
    filename_stream << trace_directory << "/alpha_" << t << ".txt";
    std::string filename = filename_stream.str();

    alpha.save(filename, arma_ascii);

    ucube rho(pvec[0].parameters.rho.n_rows, pvec[0].parameters.rho.n_cols,
             pvec.size());

    for(size_t i{}; i < pvec.size(); i++) {
      rho.slice(i) = pvec[i].parameters.rho;
    }

    std::ostringstream filename_stream_rho;
    filename_stream_rho << trace_directory << "/rho_" << t << ".txt";
    std::string filename_rho = filename_stream_rho.str();

    rho.save(filename_rho, arma_ascii);

    mat tau(pvec[0].parameters.tau.n_rows, pvec.size());
    for(size_t i{}; i < pvec.size(); i++) {
      tau.col(i) = pvec[i].parameters.tau;
    }

    std::ostringstream filename_stream_tau;
    filename_stream_tau << trace_directory << "/tau_" << t << ".txt";
    std::string filename_tau = filename_stream_tau.str();

    tau.save(filename_tau, arma_ascii);

    vec log_importance_weights(pvec.size());
    for(size_t i{}; i < pvec.size(); i++) {
      log_importance_weights(i) = pvec[i].log_importance_weight;
    }

    std::ostringstream filename_stream_log_weights;
    filename_stream_log_weights << trace_directory << "/log_weights_" << t << ".txt";
    std::string filename_log_weights = filename_stream_log_weights.str();

    log_importance_weights.save(filename_log_weights, arma_ascii);
  }
  if(trace_latent) {
    for(size_t i{}; i < pvec.size(); i++) {
      std::ostringstream filename_stream_latent_rankings;
      filename_stream_latent_rankings << trace_directory << "/latent_rankings" << t << "_" << i << ".bin";
      std::string filename_latent_rankings = filename_stream_latent_rankings.str();
      pvec[i].particle_filters[pvec[i].conditioned_particle_filter].latent_rankings.save(filename_latent_rankings);
    }
    // Create the zip file
    std::ostringstream zip_command;
    zip_command << "zip -jq " << trace_directory << "/latent_rankings" << t << ".zip " << trace_directory << "/latent_rankings" << t << "_*.bin";
    int zip_status = system(zip_command.str().c_str());
    if (zip_status != 0) {
      Rcpp::stop("Error creating zip file.");
    }

    // Delete the .bin files
    std::ostringstream rm_command;
    rm_command << "rm " << trace_directory << "/latent_rankings" << t << "_*.bin";
    int rm_status = system(rm_command.str().c_str());
    if (rm_status != 0) {
      Rcpp::stop("Error deleting .bin files.");
    }
  }
}
