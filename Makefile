all: .depend client server
DIR_PREFIX=../
include ../../MakeVars

CXXFLAGS  := -std=c++11 $(COS_CXXFLAGS) $(CXXFLAGS)
LDLIBS    := $(COS_LDLIBS) $(LDLIBS)
LDFLAGS   := $(COS_LDFLAGS) $(LDFLAGS)
DEPS      := $(COS_DEPS) $(DEPS)

INSTALL_DIR     = services/naming
INSTALL_SRCS    = Makefile account.idl client.cc server.cc
INSTALL_SCRIPTS = account_test

client: account.h account.o client.o $(DEPS)
	$(LD) $(CXXFLAGS) $(LDFLAGS) account.o client.o $(LDLIBS) -o $@ 

server: account.h account.o server.o $(DEPS)
	$(LD) $(CXXFLAGS) $(LDFLAGS) account.o server.o $(LDLIBS) -o $@ -std=c++11

account.h account.cc idl: account.idl $(IDLGEN)
	$(IDL) account.idl

clean:
	rm -f .depend account.cc account.h *.o core client server *~
