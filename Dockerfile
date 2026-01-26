# Use the CRAN debian image as the base
FROM --platform=linux/amd64 ghcr.io/r-devel/rcheckserver/debian:latest

# Set a working directory for the build
WORKDIR /build

# Copy only the DESCRIPTION file first
# This ensures R doesn't rebuild this layer unless dependencies change
COPY DESCRIPTION .

# Install the system requirements and R dependencies
RUN R -e 'install.packages(c("remotes", "rcmdcheck"), repos="https://cloud.r-project.org")' \
    && R -e 'remotes::install_deps(dependencies = TRUE)'

# Clean up the build directory
WORKDIR /workdir
