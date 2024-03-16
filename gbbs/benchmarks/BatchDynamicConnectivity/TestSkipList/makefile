# git root directory
ROOTDIR = $(strip $(shell git rev-parse --show-cdup))

include $(ROOTDIR)gbbs/makefile.variables
CFLAGS += -I$(ROOTDIR)catch2/single_include/

ALL= TestSkipList

include $(ROOTDIR)gbbs/benchmarks/makefile.benchmarks
