# Makefile
#

GLOOX =		true
SQL =		false
CXX = 		g++
LDLIBS =	-lm
ALLOBJS =	al.o patternops.o evaluate.o extend.o compress.o cluster.o globals.o ar.o ap.o ad.o sampling.o
CASASOBJS =	casasar.o casasap.o casas.o connection.o
DBOBJS =	dbar.o
TARGETS =	al
CFFLAGS =	-Wall -I/opt/local/include
CFLAGS =	-Wall -O3 -I/opt/local/include
DBFLAGS =	-I/usr/include/postgresql
LFLAGS =	-L/opt/local/lib -lgloox -lpthread
LDFLAGS =	-L/usr/lib -lpq

ifeq ($(GLOOX),true)
   ifeq ($(SQL),true)
      O = $(ALLOBJS) $(CASASOBJS) $(DBOBJS)
      FLAGS = -g $(CFFLAGS) -DGLOOX -DSQL
      LIBS = $(LDFLAGS) $(LFLAGS) $(LDLIBS)
   else
      O = $(ALLOBJS) $(CASASOBJS)
      FLAGS = -g $(CFFLAGS) -DGLOOX
      LIBS = $(LFLAGS) $(LDLIBS)
   endif
else
   ifeq ($(SQL),true)
      O = $(ALLOBJS) $(DBOBJS)
      FLAGS = -g $(CFLAGS) -DSQL
      LIBS = $(LDFLAGS) $(LDLIBS)
   else
      O = $(ALLOBJS)
      FLAGS = -g $(CFLAGS)
      LIBS = $(LDLIBS)
   endif
endif

all: $(TARGETS)

al: $(O)
	$(CXX) $(FLAGS) -o al $(O) $(LIBS)

al.o: al.c al.h
	$(CXX) $(FLAGS) -c al.c

ar.o: ar.c al.h
	$(CXX) $(FLAGS) -c ar.c

ap.o: ap.c al.h
	$(CXX) $(FLAGS) -c ap.c

ad.o: ad.c al.h
	$(CXX) $(FLAGS) -c ad.c

sampling.o: sampling.c al.h
	$(CXX) $(FLAGS) -c sampling.c

globals.o: globals.c al.h
	$(CXX) $(FLAGS) -c globals.c

patternops.o: patternops.c al.h
	$(CXX) $(FLAGS) -c patternops.c

evaluate.o: evaluate.c al.h
	$(CXX) $(FLAGS) -c evaluate.c

extend.o: extend.c al.h
	$(CXX) $(FLAGS) -c extend.c

compress.o: compress.c al.h
	$(CXX) $(FLAGS) -c compress.c

cluster.o: cluster.c al.h
	$(CXX) $(FLAGS) -c cluster.c

casasar.o: casasar.h casas.h casasar.cpp casas.o connection.o
	$(CXX) $(FLAGS) -c casasar.cpp

casasap.o: casasap.h casas.h casasap.cpp casas.o connection.o
	$(CXX) $(FLAGS) -c casasap.cpp

casas.o: casas.h casas.cpp
	$(CXX) $(FLAGS) -c casas.cpp

connection.o: casas.h connection.cpp
	$(CXX) $(FLAGS) -c connection.cpp

dbar.o: dbar.h dbar.c
	$(CXX) $(FLAGS) $(DBFLAGS) -c dbar.c

install:
	cp $(TARGETS) ../bin

clean:
	rm -f al *.o
	rm -rf *.dSYM
