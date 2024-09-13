#include "post_process.h"

using namespace arma;

mat extract_alpha_values(
    const std::vector<Particle>& particle_vector,
    const Prior& prior
) {
  mat alpha_values(prior.n_clusters, particle_vector.size());
  
  for(size_t i{}; i < particle_vector.size(); i++) {
    alpha_values.col(i) = particle_vector[i].parameters.alpha;
  }
  
  return alpha_values;
}
