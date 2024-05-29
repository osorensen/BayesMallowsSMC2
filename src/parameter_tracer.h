#include <string>
#include "particle.h"
#pragma once

struct ParameterTracer{
  ParameterTracer(bool trace, const std::string& trace_directory);
  ~ParameterTracer() = default;
  bool trace;
  std::string trace_directory;
  void update_trace(const std::vector<Particle>& pvec, int t);
};
