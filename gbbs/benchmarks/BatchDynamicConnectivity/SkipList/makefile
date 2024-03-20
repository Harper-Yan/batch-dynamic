# git root directory
ROOTDIR = $(strip $(shell git rev-parse --show-cdup))

include $(ROOTDIR)gbbs/makefile.variables

ALL= SkipList TestSkipList

include $(ROOTDIR)gbbs/benchmarks/makefile.benchmarks

