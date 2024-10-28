#include <RcppArmadillo.h>
// [[Rcpp::depends(RcppArmadillo)]]
using namespace arma;

// [[Rcpp::export]]
ivec read_vec(std::string path) {
  ivec A;
  A.load(path);
  Rcpp::Rcout << A << std::endl;
  return A;
} 

// [[Rcpp::export]]
mat read_mat(std::string path) {
  mat A;
  A.load(path, arma_ascii);
  return A;
}

// [[Rcpp::export]]
umat read_umat(std::string path) {
  umat A;
  A.load(path, auto_detect);
  return A;
}


// [[Rcpp::export]]
ucube read_cube(std::string path) {
  ucube A;
  A.load(path, arma_ascii);
  return A;
}