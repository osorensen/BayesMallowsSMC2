#include <RcppArmadillo.h>
#include "sample_latent_rankings.h"
#include "data.h"
#include "misc.h"
using namespace arma;

uvec shuffle_rcpp(const uvec& values_in) {
  ivec inds = Rcpp::sample(values_in.size(), values_in.size(), false) - 1;
  return values_in(conv_to<uvec>::from(inds));
}

LatentRankingProposal sample_latent_rankings(
    const std::unique_ptr<Data>& data, unsigned int t, const Prior& prior,
    std::string latent_rank_proposal,
    const StaticParameters& parameters,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun
) {
  if(Rankings* r = dynamic_cast<Rankings*>(data.get())) {
    return sample_latent_rankings(r, t, latent_rank_proposal,
                                  parameters, pfun, distfun);
  } else if (PairwisePreferences* pp = dynamic_cast<PairwisePreferences*>(data.get())) {
    return sample_latent_rankings(pp, t, prior);
  } else {
    Rcpp::stop("Unknown type.");
  }
}

LatentRankingProposal sample_latent_rankings(
    const Rankings* data, unsigned int t,
    std::string latent_rank_proposal,
    const StaticParameters& parameters,
    const std::unique_ptr<PartitionFunction>& pfun,
    const std::unique_ptr<Distance>& distfun)  {

  LatentRankingProposal proposal;
  ranking_tp new_data = data->timeseries[t];

  size_t proposal_index{};
  for(auto ndit = new_data.begin(); ndit != new_data.end(); ++ndit) {

    proposal.users[proposal_index] = ndit->first;
    proposal_index++;

    if(data->partial_rankings) {
      uvec tmp = ndit->second.observation;

      if(latent_rank_proposal == "uniform") {
        tmp(ndit->second.available_items) = shuffle_rcpp(ndit->second.available_rankings);
        proposal.proposal = join_horiz(proposal.proposal, tmp);
        proposal.log_probability = join_vert(
          proposal.log_probability, vec{-lgamma(ndit->second.available_rankings.size() + 1.0)});
      } else if(latent_rank_proposal == "pseudo") {
        if(parameters.alpha.size() > 1) {
          Rcpp::stop("Pseudolikelihood proposal does not work with clusters.");
        }
        double logprob{0};

        uvec available_items_shuffled = shuffle_rcpp(ndit->second.available_items);
        uvec available_rankings = ndit->second.available_rankings;

        while(available_items_shuffled.size() > 1) {
          uvec rhocol = parameters.rho.col(0);
          uvec rho0 = rhocol.rows(available_items_shuffled);

          vec probs = exp(softmax(-parameters.alpha(0) * abs(conv_to<vec>::from(rho0) - conv_to<vec>::from(available_rankings))));

          Rcpp::IntegerVector sampled_index = Rcpp::sample(
            probs.size(), 1, true, Rcpp::as<Rcpp::NumericMatrix>(Rcpp::wrap(probs)), false);

          tmp(available_items_shuffled(0)) = available_rankings(sampled_index(0));
          logprob += log(probs(sampled_index(0)));
          available_items_shuffled = available_items_shuffled(span(1, available_items_shuffled.size() - 1));
          available_rankings = setdiff(available_rankings, uvec{available_rankings(sampled_index(0))});
        }

        if(available_items_shuffled.size() == 1) {
          tmp(available_items_shuffled(0)) = available_rankings(0);
        }

        if(!approx_equal(sort(tmp), regspace<uvec>(1, tmp.size()), "absdiff", 0)) {
          Rcpp::stop("Not a ranking.");
        }

        proposal.proposal = join_horiz(proposal.proposal, tmp);
        proposal.log_probability = join_vert(proposal.log_probability, vec{logprob});

      } else {
        Rcpp::stop("Unknown latent rank proposal.");
      }
    } else {
      proposal.proposal = ndit->second.observation;
      proposal.log_probability = join_vert(proposal.log_probability, vec{0});
    }


    if(parameters.tau.size() > 1) {
      vec log_cluster_probabilities(parameters.tau.size());

      for(size_t cluster{}; cluster < parameters.tau.size(); cluster++) {
        log_cluster_probabilities(cluster) = log(parameters.tau(cluster)) -
          pfun->logz(parameters.alpha(cluster)) - parameters.alpha(cluster) *
          distfun->d(proposal.proposal, parameters.rho.col(cluster));
      }

      log_cluster_probabilities = softmax(log_cluster_probabilities);
      proposal.cluster_probabilities = join_rows(proposal.cluster_probabilities, exp(log_cluster_probabilities));

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
  pairwise_tp new_data = data->timeseries[t];
  sort_matrices_tp new_sort_matrices = data->sort_matrix_timeseries[t];
  sort_counts_tp new_sort_counts = data->sort_count_timeseries[t];
  size_t proposal_index{};

  for(auto ndit = new_data.begin(); ndit != new_data.end(); ++ndit) {
    umat sort_matrix = new_sort_matrices[ndit->first];
    int random_index = Rcpp::sample(sort_matrix.n_cols, 1, false)[0] - 1;

    proposal.proposal.col(proposal_index++) = sort_matrix.col(random_index);
    proposal.log_probability = join_vert(
      proposal.log_probability, vec{-log(new_sort_counts[ndit->first])}
    );
  }

  return proposal;
}
