#include <string>
#include "parameter_tracer.h"
using namespace arma;

ParameterTracer::ParameterTracer(bool trace, bool trace_latent)
  : trace { trace }, trace_latent { trace_latent } {}

void ParameterTracer::update_trace(const std::vector<Particle>& pvec, int t) {
  if(trace) {
    mat alpha(pvec[0].parameters.alpha.n_rows, pvec.size());
    for(size_t i{}; i < pvec.size(); i++) {
      alpha.col(i) = pvec[i].parameters.alpha;
    }
    alpha_traces.push_back(alpha);

    ucube rho(pvec[0].parameters.rho.n_rows, pvec[0].parameters.rho.n_cols,
              pvec.size());

    for(size_t i{}; i < pvec.size(); i++) {
      rho.slice(i) = pvec[i].parameters.rho;
    }
    rho_traces.push_back(rho);

    mat tau(pvec[0].parameters.tau.n_rows, pvec.size());
    for(size_t i{}; i < pvec.size(); i++) {
      tau.col(i) = pvec[i].parameters.tau;
    }
    tau_traces.push_back(tau);

    vec log_importance_weights(pvec.size());
    for(size_t i{}; i < pvec.size(); i++) {
      log_importance_weights(i) = pvec[i].log_importance_weight;
    }
    log_importance_weights_traces.push_back(log_importance_weights);
  }
  if(trace_latent) {
    std::vector<arma::umat> current_latent_rankings;
    for(size_t i{}; i < pvec.size(); i++) {
      current_latent_rankings.push_back(pvec[i].particle_filters[pvec[i].conditioned_particle_filter].latent_rankings);
    }
    latent_rankings_traces.push_back(current_latent_rankings);
  }
}
