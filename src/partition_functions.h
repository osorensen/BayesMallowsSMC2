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

struct Cardinalities : PartitionFunction {
  Cardinalities(unsigned int n_items, const std::string& metric);
  double logz(double alpha) override;
  const unsigned int n_items;
  arma::vec distances;
  arma::vec cardinalities;
};
