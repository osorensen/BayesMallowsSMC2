// Generated by using Rcpp::compileAttributes() -> do not edit by hand
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <RcppArmadillo.h>
#include <Rcpp.h>

using namespace Rcpp;

#ifdef RCPP_USE_GLOBAL_ROSTREAM
Rcpp::Rostream<true>&  Rcpp::Rcout = Rcpp::Rcpp_cout_get();
Rcpp::Rostream<false>& Rcpp::Rcerr = Rcpp::Rcpp_cerr_get();
#endif

// precompute_topological_sorts
void precompute_topological_sorts(const arma::umat& prefs, int n_items, std::string output_directory, int max_files_to_save);
RcppExport SEXP _BayesMallowsSMC2_precompute_topological_sorts(SEXP prefsSEXP, SEXP n_itemsSEXP, SEXP output_directorySEXP, SEXP max_files_to_saveSEXP) {
BEGIN_RCPP
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< const arma::umat& >::type prefs(prefsSEXP);
    Rcpp::traits::input_parameter< int >::type n_items(n_itemsSEXP);
    Rcpp::traits::input_parameter< std::string >::type output_directory(output_directorySEXP);
    Rcpp::traits::input_parameter< int >::type max_files_to_save(max_files_to_saveSEXP);
    precompute_topological_sorts(prefs, n_items, output_directory, max_files_to_save);
    return R_NilValue;
END_RCPP
}
// run_smc
Rcpp::List run_smc(Rcpp::List input_timeseries, Rcpp::List input_prior, Rcpp::List input_options);
RcppExport SEXP _BayesMallowsSMC2_run_smc(SEXP input_timeseriesSEXP, SEXP input_priorSEXP, SEXP input_optionsSEXP) {
BEGIN_RCPP
    Rcpp::RObject rcpp_result_gen;
    Rcpp::RNGScope rcpp_rngScope_gen;
    Rcpp::traits::input_parameter< Rcpp::List >::type input_timeseries(input_timeseriesSEXP);
    Rcpp::traits::input_parameter< Rcpp::List >::type input_prior(input_priorSEXP);
    Rcpp::traits::input_parameter< Rcpp::List >::type input_options(input_optionsSEXP);
    rcpp_result_gen = Rcpp::wrap(run_smc(input_timeseries, input_prior, input_options));
    return rcpp_result_gen;
END_RCPP
}

static const R_CallMethodDef CallEntries[] = {
    {"_BayesMallowsSMC2_precompute_topological_sorts", (DL_FUNC) &_BayesMallowsSMC2_precompute_topological_sorts, 4},
    {"_BayesMallowsSMC2_run_smc", (DL_FUNC) &_BayesMallowsSMC2_run_smc, 3},
    {NULL, NULL, 0}
};

RcppExport void R_init_BayesMallowsSMC2(DllInfo *dll) {
    R_registerRoutines(dll, NULL, CallEntries, NULL, NULL);
    R_useDynamicSymbols(dll, FALSE);
}
