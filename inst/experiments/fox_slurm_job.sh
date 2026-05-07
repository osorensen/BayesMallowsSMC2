#!/bin/bash
#SBATCH --job-name=pg_bsi_sim
#SBATCH --account=ec-oysteis
#SBATCH --partition=normal
#SBATCH --nodes=1
#SBATCH --ntasks-per-node=1
#SBATCH --cpus-per-task=16
#SBATCH --time=02:00:00
#SBATCH --mem=32G
#SBATCH --output=slurm-%j.out

# Load necessary modules
source /etc/profile.d/modules.sh
module purge
module load R/4.4.1-gfbf-2023b # Example standard module on Fox, or we can use local R if needed

cd ~/code/BayesMallowsSMC2

# Install the updated package into the local library
mkdir -p ~/R/x86_64-pc-linux-gnu-library/4.4
export R_LIBS_USER="~/R/x86_64-pc-linux-gnu-library/4.4"
R CMD INSTALL .

# Run the experiment
Rscript inst/experiments/compare_pg_bsi.R
