CXX = g++ -std=c++11

SRCS := $(wildcard *.cpp ) 
OBJECTS := $(subst .cpp,.o,$(SRCS))

INC =

LIBNAME = libkafkaprotocpp.a

CFLAGS = -ggdb -Wno-deprecated -fPIC -O2
LFLAGS = -shared -Wl,-soname,$(LIB_SONAME)
SFLAGS = rcs

$(LIBNAME): $(OBJECTS)
	$(AR) $(SFLAGS) $(LIBNAME) $^

%.o:%.cpp
	$(CXX) $(CFLAGS) -c $(INC) -o $@ $<

.PHONY: clean
clean:
	rm -f $(OBJECTS) $(LIBNAME)
