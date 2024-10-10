#include <RcppArmadillo.h>
#include <filesystem>
#include <vector>
#include "sample_latent_rankings.h"
#include "data.h"
#include "misc.h"
using namespace arma;
namespace fs = std::filesystem;

LatentRankingProposal sample_latent_rankings(
    const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior,
    std::string latent_rank_proposal,
    const StaticParameters& parameters,
    const umat& current_latent_rankings,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun
) {
  if(Rankings* r = dynamic_cast<Rankings*>(data.get())) {
    return sample_latent_rankings(r, t, prior, latent_rank_proposal,
                                  parameters, current_latent_rankings, pfun, distfun);
  } else if (PairwisePreferences* pp = dynamic_cast<PairwisePreferences*>(data.get())) {
    return sample_latent_rankings(pp, t, prior);
  } else {
    Rcpp::stop("Unknown type.");
  }
}

LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t, const Prior& prior,
    std::string latent_rank_proposal,
    const StaticParameters& parameters,
    const umat& current_latent_rankings,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun)  {
  LatentRankingProposal proposal;
  ranking_tp new_data = data->timeseries[t];
  uvec all_items = regspace<uvec>(0, prior.n_items - 1);
  uvec all_rankings = regspace<uvec>(1, prior.n_items);

  for(size_t i{}; i < new_data.size(); i++) {
    auto it = std::find(data->observed_users.begin(), data->observed_users.end(), new_data[i].first);
    if(it == data->observed_users.end()) {
      proposal.new_users.push_back(new_data[i].first);
    } else {
      int lr_index = std::distance(data->observed_users.begin(), it);

      uvec new_observed_indices = find(new_data[i].second); // indices of non-missing ranks
      uvec v1 = new_data[i].second(new_observed_indices); // ranks of ranked items
      uvec v2 = current_latent_rankings.col(lr_index); // current latent ranking
      v2 = v2.rows(new_observed_indices); // latent rankings limited to new observed
      if(all(v1 == v2)) { // observations agree with latent
        proposal.updated_consistent_users.push_back(new_data[i].first);
        continue;
      } else { // observations don't agree with latent
        proposal.updated_inconsistent_users.push_back(new_data[i].first);
      }
    }

    uvec observed_ranking = new_data[i].second;
    uvec observed_items = find(observed_ranking);
    uvec available_items = setdiff(all_items, observed_items);
    uvec available_rankings = setdiff(all_rankings, observed_ranking);

    uvec tmp = observed_ranking;

    if(latent_rank_proposal == "uniform") {
      tmp(available_items) = shuffle(available_rankings);
      proposal.proposal = join_horiz(proposal.proposal, tmp);
      proposal.log_probability -= lgamma(available_rankings.size() + 1.0);
    } else if(latent_rank_proposal == "pseudo") {
      if(parameters.alpha.size() > 1) {
        Rcpp::stop("Pseudolikelihood proposal does not work with clusters.");
      }
      double logprob{0};

      uvec available_items_shuffled = shuffle(available_items);

      while(available_items_shuffled.size() > 1) {
        uvec rho0(available_items_shuffled.size());
        for(size_t i{}; i < rho0.size(); i++) {
          unsigned int item = available_items_shuffled(i);
          rho0(i) = parameters.rho(item, 0);
        }
        double alpha0 = parameters.alpha(0);
        vec probs = exp(-alpha0 * abs(conv_to<vec>::from(rho0) - conv_to<vec>::from(available_rankings)));
        probs = normalise(probs, 1);
        Rcpp::IntegerVector sampled_index = Rcpp::sample(
          probs.size(), 1, true, Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(probs)),
          false
        );
        tmp(available_items_shuffled(0)) = available_rankings(sampled_index(0));
        logprob += log(probs(sampled_index(0)));
        available_items_shuffled = available_items_shuffled(span(1, available_items_shuffled.size() - 1));
        available_rankings = setdiff(available_rankings, uvec{available_rankings(sampled_index(0))});
      }

      if(available_items_shuffled.size() == 1) {
        tmp(available_items_shuffled(0)) = available_rankings(0);
        available_items.reset();
        available_rankings.reset();
      }

      if(!approx_equal(sort(tmp), regspace<uvec>(1, tmp.size()), "absdiff", 0)) {
        Rcpp::stop("Not a ranking.");
      }

      proposal.proposal = join_horiz(proposal.proposal, tmp);
      proposal.log_probability += logprob;

    } else {
      Rcpp::stop("Unknown latent rank proposal.");
    }

    if(it == data->observed_users.end()) {
      vec log_cluster_probabilities(parameters.tau.size());

      for(size_t cluster{}; cluster < parameters.tau.size(); cluster++) {
        log_cluster_probabilities(cluster) = log(parameters.tau(cluster)) -
          pfun->logz(parameters.alpha(cluster)) - parameters.alpha(cluster) *
          distfun->d(proposal.proposal, parameters.rho.col(cluster));
      }

      log_cluster_probabilities = log_sum_exp(log_cluster_probabilities);

      unsigned int z = Rcpp::sample(
        parameters.tau.size(), 1, false,
        Rcpp::as<Rcpp::NumericVector>(Rcpp::wrap(exp(log_cluster_probabilities))),
        false
      )(0);
      proposal.cluster_assignment = join_vert(proposal.cluster_assignment, uvec{z});
    }
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
