#pragma once
#include <RcppArmadillo.h>
#include <string>

struct Data {
  Data() {};
  virtual ~Data() = default;
  virtual int n_timepoints() = 0;
  virtual int n_users() = 0;
  virtual std::string type() const = 0;
  virtual arma::imat get_timepoint_data(int t) = 0;
  virtual arma::uvec get_users_at_timepoint(int t) = 0;
  virtual arma::uvec get_users_before_timepoint(int t) = 0;
  virtual arma::ivec get_user_data(int t, int u) = 0;
};

struct MatrixData : Data {
  arma::imat matrix;
  
  MatrixData(const Rcpp::List& input_data, const std::string& key);
  int n_timepoints() override;
  int n_users() override;
  arma::imat get_timepoint_data(int t) override;
  arma::uvec get_users_at_timepoint(int t) override;
  arma::uvec get_users_before_timepoint(int t) override;
  arma::ivec get_user_data(int t, int u) override;
};

struct CompleteRankings : MatrixData {
  CompleteRankings(const Rcpp::List& input_data);
  std::string type() const override { return "CompleteRankings"; }
};

struct PartialRankings : MatrixData {
  PartialRankings(const Rcpp::List& input_data);
  std::string type() const override { return "PartialRankings"; }
};

struct Preferences : MatrixData {
  Preferences(const Rcpp::List& input_data);
  std::string type() const override { return "Preferences"; }
};

std::unique_ptr<Data> setup_data(const Rcpp::List& input_data);

