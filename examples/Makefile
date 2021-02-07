CXX = g++ -std=c++11

SRCS := $(wildcard *.cpp)
OBJECTS := $(subst .cpp,.o,$(SRCS))

INC =

CFLAGS = -ggdb -Wno-deprecated -fPIC -O2
LFLAGS = 
SFLAGS = rcs

target := meta_query

LIBS = ../libkafkaprotocpp.a
meta_query: meta_query.cpp
	$(CXX) $(LFLAGS) -o $(target) $^ $(LIBS)

%.o:%.cpp
	$(CXX) $(CFLAGS) -c $(INC) -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(target) 
