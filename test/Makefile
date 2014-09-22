GTEST_DIR=/Users/rex/CS/repos/googlemock-read-only/gtest
GTEST_LIBDIR=$(GTEST_DIR)/mybuild
GTEST_INCDIR=$(GTEST_DIR)/include
COMMFLAGS=-g
#COMMFLAGS=-g -DUSE_SYS_GETOPT
CXXFLAGS=-I$(GTEST_INCDIR) $(COMMFLAGS)
CXX=g++
CFLAGS=$(COMMFLAGS) -std=c99
CC=gcc

HEADERS=../src/my_getopt.h  ../src/my_linklist.h ../src/my_config.h
CXXSRCS=my-getopt_test.cc my-linklist_test.cc my-config_test.cc calculate-sum_test.cc
CSRCS=../src/my_getopt.c ../src/my_linklist.c ../src/my_config.c ../src/calculate_sum.c
CXXOBJS=$(patsubst %.cc, %.o, $(notdir $(CXXSRCS)))
COBJS=$(patsubst %.c, %.o, $(notdir $(CSRCS)))
TARGET=test_main
DEPS=$(HEADERS)

all: $(TARGET)

$(CXXOBJS): $(CXXSRCS) $(DEPS)
	$(CXX) $(CXXFLAGS) -c $(CXXSRCS)

$(COBJS): $(CSRCS)
	$(CC) $(CFLAGS) -c $(CSRCS)

$(TARGET): $(COBJS) $(CXXOBJS)
	$(CXX) -o $@ $^ $(GTEST_LIBDIR)/libgtest_main.a $(GTEST_LIBDIR)/libgtest.a

.PHONY:
	clean distclean
clean:
	rm -f *.o *.gch

distclean: clean
	rm -f $(TARGET)
