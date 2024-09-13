#include "post_process.h"

using namespace arma;

mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
) {
  mat alpha_values(particle_vector.size(), prior.n_clusters);
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha_values.row(i) = particle_vector[i].parameters.alpha.t();
  }
  
  return alpha_values;
}
