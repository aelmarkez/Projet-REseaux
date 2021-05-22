export CC=gcc
export CFLAGS=-Wall -Wextra -std=gnu99 -pthread -g -O0
PWD=$(shell pwd)
export BUILD_DIR=$(PWD)/build



all: controller


controller: build_dir
	@make -C src/$@


build_dir:
	mkdir -p $(BUILD_DIR)


clean:
	@make clean -C src/controller
	rm -rf build/*
	rm -rf *.log*
