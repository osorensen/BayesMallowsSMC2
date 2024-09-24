#include "distances.h"
#include "leap_and_shift.h"
#include "setdiff.h"

using namespace arma;

ivec leap_and_shift(const ivec& current_rho) {
  int n_items = current_rho.size();
  ivec proposal = current_rho;
  int u = randi(distr_param(0, n_items - 1));
  
  ivec support = join_cols(
    regspace<ivec>(std::max(1, current_rho(u) - 1), +1, current_rho(u) - 1),
    regspace<ivec>(current_rho(u) + 1, +1, std::min(n_items, current_rho(u) + 1)));
  
  int index = randi(distr_param(0, support.n_elem - 1));
  proposal(u) = support(index);
  
  int delta_r = proposal(u) - current_rho(u);
  
  if(delta_r > 0) {
    for(int k{1}; k <= delta_r; k++) {
      int item_index = as_scalar(find(current_rho == current_rho(u) + k));
      proposal(item_index) -= 1;
    }
  } else if(delta_r < 0) {
    for(int k{-1}; k >= delta_r; k--) {
      int item_index = as_scalar(find(current_rho == current_rho(u) + k));
      proposal(item_index) += 1;
    }
  }
  
  return proposal;
}
  