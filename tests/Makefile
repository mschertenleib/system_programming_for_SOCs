PROJECT = tests

CFLAGS = -Wfatal-errors -Wall -Wextra -Wshadow -Wcast-align -Wunused -Wpedantic\
	-Wconversion -Wnull-dereference -Wdouble-promotion -Wmisleading-indentation\
    -Wduplicated-cond -Wduplicated-branches -Wlogical-op
CFLAGS += -O3

COMMON_SRCS = $(wildcard common/*.c)
COMMON_OBJS = $(COMMON_SRCS:%.c=build/%.c.o)

all: build/float build/fixed build/myfloat

build/float: build/src/float.c.o $(COMMON_OBJS)
	@mkdir -p $(@D)
	@gcc $^ -o $@

build/fixed: build/src/fixed.c.o $(COMMON_OBJS)
	@mkdir -p $(@D)
	@gcc $^ -o $@

build/myfloat: build/src/myfloat.c.o $(COMMON_OBJS)
	@mkdir -p $(@D)
	@gcc $^ -o $@

# Build stb_image_write without warnings
build/common/stb_image_write.c.o: common/stb_image_write.c
	@mkdir -p $(@D)
	@gcc -c $< -o $@ -Icommon

build/%.c.o: %.c
	@mkdir -p $(@D)
	@gcc $(CFLAGS) -c $< -o $@ -Icommon


.PHONY: clean

clean:
	@rm -r build/*
