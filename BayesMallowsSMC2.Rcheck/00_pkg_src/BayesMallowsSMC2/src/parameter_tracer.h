#include <string>
#include "particle.h"
#pragma once

struct ParameterTracer{
  ParameterTracer(bool trace, bool trace_latent);
  ~ParameterTracer() = default;
  bool trace;
  bool trace_latent;
  std::vector<arma::mat> alpha_traces{};
  std::vector<arma::ucube> rho_traces{};
  std::vector<arma::mat> tau_traces{};
  std::vector<arma::vec> log_importance_weights_traces{};
  std::vector<std::vector<arma::umat>> latent_rankings_traces{};
  void update_trace(const std::vector<Particle>& pvec, int t);
};
