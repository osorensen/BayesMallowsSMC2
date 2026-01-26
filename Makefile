IMAGE_NAME=bayesmallows-check-env
PLATFORM=linux/amd64
VOL=-v "$(shell pwd)":/workdir
WORKDIR=-w /workdir

.PHONY: build-env check-debian shell clean

# 1. Build the environment (run this when DESCRIPTION changes)
build-env:
	docker build --platform $(PLATFORM) -t $(IMAGE_NAME) .

# 2. Run the check (fast, uses cached dependencies)
check-debian:
	docker run --rm $(VOL) $(WORKDIR) --platform $(PLATFORM) $(IMAGE_NAME) \
		bash -c "R CMD build . && R CMD check BayesMallowsSMC2_*.tar.gz"

# 3. Debugging shell
shell:
	docker run --rm -it $(VOL) $(WORKDIR) --platform $(PLATFORM) $(IMAGE_NAME) /bin/bash

# 4. Clean up check artifacts
clean:
	rm -rf *.Rcheck/
	rm -f *.tar.gz
