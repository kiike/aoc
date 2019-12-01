all: 1
test:
	test $(shell echo 12 | ./1 -n) == 2
	test $(shell echo 14 | ./1 -n) == 2
	test $(shell echo 1969 | ./1 -c) == 966
	test $(shell echo 100756 | ./1 -c) == 50346

.PHONY: test all
