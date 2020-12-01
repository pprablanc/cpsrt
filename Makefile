#
#
#
# Makefile template for C code
#
# Author: Gustavo Pantuza Coelho Pinto
# Since: 24.03.2016
#
#
#


# Includes the project configurations
include project.conf

#
# Validating project variables defined in project.conf
#
ifndef PROJECT_NAME
$(error Missing PROJECT_NAME. Put variables at project.conf file)
endif
ifndef BINARY
$(error Missing BINARY. Put variables at project.conf file)
endif
ifndef PROJECT_PATH
$(error Missing PROJECT_PATH. Put variables at project.conf file)
endif

# Gets the Operating system name
OS := $(shell uname -s)

# Default shell
SHELL := bash

# Color prefix for Linux distributions
COLOR_PREFIX := e

ifeq ($(OS),Darwin)
	COLOR_PREFIX := 033
endif

# Color definition for print purpose
BROWN=\$(COLOR_PREFIX)[0;33m
BLUE=\$(COLOR_PREFIX)[1;34m
END_COLOR=\$(COLOR_PREFIX)[0m


# Source code directory structure
#BINDIR := bin
SRCDIR := src
OBJDIR := obj
LIBDIR := include
#LOGDIR := log
#TESTDIR := test

# Source code file extension
SRCEXT := c

# Defines the C compiler
CC := gcc

CFLAGS := -no-pie

# Dependency libraries
LIBS := lib/libportaudio.a -lrt -lasound -lpthread -lm -ljack

# %.o file names
NAMES := $(notdir $(basename $(wildcard $(SRCDIR)/*.$(SRCEXT))))
OBJECTS :=$(patsubst %,$(OBJDIR)/%.o,$(NAMES))

default: all
recompile: clean all

# Linking rules and binary file generation
all: $(OBJECTS)
	@echo -en "$(BROWN)LD $(END_COLOR)";
	$(CC) -o $(BINARY) $+ $(CFLAGS) $(LIBS)

#.o: %.c
#	gcc -c $(LIBS) $<

# Rules for object binaries compilation
$(OBJDIR)/%.o: $(SRCDIR)/%.$(SRCEXT)
	@echo -en "$(BROWN)CC $(END_COLOR)";
	$(CC) -c $^ -o $@

# clean recorded sessions and objects
clean:
	rm -rvf check_output check_input
	rm -rvf $(OBJDIR)/*
