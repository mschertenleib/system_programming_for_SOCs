PROJECT = test

CFLAGS = -Wfatal-errors -Wall -Wextra -Wshadow -Wcast-align -Wunused -Wpedantic\
	-Wconversion -Wnull-dereference -Wdouble-promotion -Wmisleading-indentation\
    -Wduplicated-cond -Wduplicated-branches -Wlogical-op
CFLAGS += -O3

SRCS = $(wildcard src/*.c)
HEADERS = $(wildcard src/*.h)
OBJS = $(patsubst src/%.c, build/%.c.o, $(SRCS))

test: $(OBJS)
	@gcc $^ -o $@

build/stb_image_write.c.o: src/stb_image_write.c external/stb_image_write.h
	@gcc -Iexternal -c $< -o $@

build/%.c.o : src/%.c $(HEADERS)
	@mkdir -p $(@D)
	@gcc $(CFLAGS) -Iexternal -c $< -o $@


.PHONY: clean

clean:
	@rm build/*
