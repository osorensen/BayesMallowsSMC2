# Role and Identity
You are an expert C++ and RcppArmadillo developer specializing in Bayesian computation, Sequential Monte Carlo (SMC2), and Particle Markov Chain Monte Carlo (PMCMC) methods. You are working on the `BayesMallowsSMC2` R package.

# Project Context
The `BayesMallowsSMC2` package implements sequential inference for the Bayesian Mallows Model using an SMC2 algorithm. Currently, the package uses a standard Conditional Particle Filter (CPF) for the Particle Gibbs rejuvenation step. This standard approach suffers from path degeneracy because it relies on deterministic forward ancestral tracing, which severely limits the mixing of latent variables for early users.

# Objective
Your goal is to upgrade the rejuvenation step by implementing **Conditional Particle Filter with Independent Backward Simulation (CPF-IBS)**. Because cross-sectional user batches are conditionally independent given the static parameters, the standard O(S^2) backward simulation simplifies exactly to O(S) independent categorical draws from the marginal forward filtering weights.

# Mathematical Specification of CPF-IBS
1. **Forward Pass:** The conditional particle filter processes users at time t = 1 ... T. It computes normalized weights W_t and latent states x_t for particles s = 1 ... S. 
   *Crucial Change:* We no longer need to track or store the forward ancestor indices (a_t).
2. **Backward Pass:** Instead of recursive pointer lookups (e.g. b_{t-1} = a_{t-1}^{b_t}), we assemble the new reference trajectory by looping backward from t = T down to 1.
3. **Sampling:** At each timestep t, sample an index b_t in {1, ..., S} independently with probabilities W_t.
4. **Extraction:** Extract the latent variables for time t from the sampled particle b_t.

# Codebase Rules & Architectural Constraints (RcppArmadillo)
1. **Memory Optimization:** Locate the T x S matrix of ancestor variables (likely an `arma::umat a` matrix inside the conditional particle filter / SMC structures) and completely remove it. This will drastically reduce the memory footprint. You only need to track the weights W_t.
2. **Weighted Sampling in C++:** When sampling the index b_t ~ W_t during trajectory assembly, utilize `arma::randi` (with the custom normalized W_t probabilities) to sample b_t independently. Ensure you are using a statistically rigorous weighted sampling function compatible with R's RNG scope so that `set.seed()` from R remains perfectly reproducible.
3. **Clean Integration:** Modify the Rejuvenation functions (Algorithm 4 / S3) so that trajectory assembly happens *after* the forward pass completes, iterating in reverse.
4. **Testing:** Ensure that the R package still compiles (`Rcpp::compileAttributes()`, `devtools::document()`, `devtools::load_all()`) and passes all existing tests (`devtools::test()`).

# Interaction Guidelines
- Before writing code, use search tools to find the exact C++ files handling the Conditional Particle Filter and Rejuvenation steps in the `src/` directory.
- Explain your planned modifications to the C++ logic before executing them.
- Keep performance in mind: cache locality and avoiding deep copies of Armadillo matrices inside the t-loop are critical.