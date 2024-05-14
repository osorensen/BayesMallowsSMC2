#include "particle.h"
#pragma once

struct ParameterTracer{
  ParameterTracer(bool trace);
  ~ParameterTracer() = default;
  bool trace;
  void update_trace(const std::vector<Particle>& pvec, int t);
};
