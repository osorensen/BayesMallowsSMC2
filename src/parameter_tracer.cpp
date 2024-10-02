#include <sys/stat.h>
#include <cstdlib>
#include <sstream>
#include "parameter_tracer.h"
using namespace arma;

ParameterTracer::ParameterTracer(bool trace, bool trace_latent, const std::string& trace_directory)
  : trace { trace }, trace_latent { trace_latent }, trace_directory { trace_directory } {
  if(trace) {
    int status = mkdir(trace_directory.c_str(), 0777);
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
  }
  if(trace_latent) {
    for(size_t i{}; i < pvec.size(); i++) {
      Rcpp::NumericVector probs = Rcpp::exp(pvec[i].log_normalized_particle_filter_weights);
      int sampled_index = Rcpp::sample(probs.size(), 1, false, probs, false)[0];

      std::ostringstream filename_stream_latent_rankings;
      filename_stream_latent_rankings << trace_directory << "/latent_rankings" << t << "_" << i << ".txt";
      std::string filename_latent_rankings = filename_stream_latent_rankings.str();
      pvec[i].particle_filters[sampled_index].latent_rankings.save(filename_latent_rankings, arma_ascii);
    }
  }
}
