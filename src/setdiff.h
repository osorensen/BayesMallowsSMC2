#include <armadillo>
#include <algorithm>
#include <vector>
#include <type_traits>

template <typename T>
T setdiff(const T& x1, const T& x2) {
  using elem_type = typename T::elem_type;
  
  std::vector<elem_type> x1_vec(x1.begin(), x1.end());
  std::vector<elem_type> x2_vec(x2.begin(), x2.end());
  
  std::sort(x1_vec.begin(), x1_vec.end());
  std::sort(x2_vec.begin(), x2_vec.end());
  
  std::vector<elem_type> difference;
  
  std::set_difference(
    x1_vec.begin(), x1_vec.end(),
    x2_vec.begin(), x2_vec.end(),
    std::back_inserter(difference)
  );
  
  return T(difference);
}