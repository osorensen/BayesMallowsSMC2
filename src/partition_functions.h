#pragma once

#include <memory>
#include <RcppArmadillo.h>

struct PartitionFunction {
  PartitionFunction() {};
  virtual ~PartitionFunction() = default;
  virtual double logz(double alpha) = 0;
};

std::unique_ptr<PartitionFunction> choose_partition_function(int n_items, std::string metric);

struct Cayley : PartitionFunction {
  Cayley(unsigned int n_items);
  double logz(double alpha) override;
  const unsigned int n_items;
};

struct Hamming : PartitionFunction {
  Hamming(unsigned int n_items);
  double logz(double alpha) override;
  const unsigned int n_items;
};

struct Kendall : PartitionFunction {
  Kendall(unsigned int n_items);
  double logz(double alpha) override;
  const unsigned int n_items;
};

struct Footrule : PartitionFunction {
  Footrule(unsigned int n_items);
  double logz(double alpha) override;
  const unsigned int n_items;
  arma::uvec cardinalities;
  arma::uvec distances;
};

struct Spearman : PartitionFunction {
  Spearman(unsigned int n_items);
  double logz(double alpha) override;
  const unsigned int n_items;
  arma::uvec cardinalities;
  arma::uvec distances;
};

struct Ulam : PartitionFunction {
  Ulam(unsigned int n_items);
  double logz(double alpha) override;
  const unsigned int n_items;
  arma::uvec cardinalities;
  arma::uvec distances;
};
