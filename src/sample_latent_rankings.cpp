#include <RcppArmadillo.h>
#include <filesystem>
#include <vector>
#include "sample_latent_rankings.h"
#include "data.h"
#include "misc.h"
using namespace arma;
namespace fs = std::filesystem;

LatentRankingProposal sample_latent_rankings(
    const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior
) {
  if(Rankings* r = dynamic_cast<Rankings*>(data.get())) {
    return sample_latent_rankings(r, t, prior);
  } else if (PairwisePreferences* pp = dynamic_cast<PairwisePreferences*>(data.get())) {
    return sample_latent_rankings(pp, t, prior);
  } else {
    Rcpp::stop("Unknown type.");
  }
}

LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t, const Prior& prior) {
  LatentRankingProposal proposal;
  ranking_tp new_data = data->timeseries[t];
  uvec all_items = regspace<uvec>(0, prior.n_items - 1);
  uvec all_rankings = regspace<uvec>(1, prior.n_items);
  proposal.proposal = umat(prior.n_items, new_data.size());

  for(size_t i{}; i < new_data.size(); i++) {
    uvec observed_ranking = new_data[i].second;
    uvec observed_items = find(observed_ranking);
    uvec available_items = setdiff(all_items, observed_items);
    uvec available_rankings = setdiff(all_rankings, observed_ranking);
    uvec tmp = observed_ranking;
    tmp(available_items) = shuffle(available_rankings);
    proposal.proposal.col(i) = tmp;

    proposal.log_probability -= lgamma(available_rankings.size() + 1.0);
  }

  return proposal;
}

LatentRankingProposal sample_latent_rankings(
    const PairwisePreferences* data, unsigned int t, const Prior& prior) {
  LatentRankingProposal proposal;
  proposal.proposal = umat(prior.n_items, data->timeseries[t].size());

  for(size_t i{}; i < data->timeseries[t].size(); i++) {
    std::string user = data->timeseries[t][i].first;
    std::string directory_path = data->topological_sorts_directory + std::string("/user") + user;
    std::vector<fs::path> file_paths;

    for(const auto& entry : fs::directory_iterator(directory_path)) {
      if(entry.is_regular_file()) {
        file_paths.push_back(entry.path());
      }
    }

    if(file_paths.empty()) {
      Rcpp::stop("Found no files.");
    }

    int file_index = randi(distr_param(0, file_paths.size() - 1));
    fs::path random_file_path = file_paths[file_index];

    ivec tmp;
    bool ok = tmp.load(random_file_path.string(), arma_binary);
    if(ok == false) {
      Rcpp::stop("Could not find file.");
    }

    Rcpp::CharacterVector nn = data->num_topological_sorts.names();
    Rcpp::IntegerVector matching_index = Rcpp::match(Rcpp::CharacterVector::create(user), nn) - 1;
    proposal.proposal.col(i) = conv_to<uvec>::from(tmp);
    proposal.log_probability = -log(data->num_topological_sorts[matching_index[0]]);
  }

  return proposal;
}
