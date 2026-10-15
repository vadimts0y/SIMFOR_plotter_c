CXX ?= g++
AR ?= ar
CPPFLAGS ?=
CXXFLAGS ?= -std=c++11 -Wall -Wextra
LDFLAGS ?=
LDLIBS ?= -lGL -lglut

.PHONY: all check clean

all: libplotter.a test_2d test_3d test_points test_core

libplotter.a: plotter.o plotter_core.o
	$(AR) rcs $@ $^

plotter.o: plotter.cpp plotter.h plotter_core.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

plotter_core.o: plotter_core.cpp plotter_core.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

test_2d: test_2d.o libplotter.a
	$(CXX) $(LDFLAGS) -o $@ test_2d.o libplotter.a $(LDLIBS)

test_3d: test_3d.o libplotter.a
	$(CXX) $(LDFLAGS) -o $@ test_3d.o libplotter.a $(LDLIBS)

test_points: test_points.o libplotter.a
	$(CXX) $(LDFLAGS) -o $@ test_points.o libplotter.a $(LDLIBS)

test_core: test_core.o plotter_core.o
	$(CXX) $(LDFLAGS) -o $@ $^

check: test_core
	./test_core

test_2d.o: test_2d.cpp plotter.h plotter_core.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

test_3d.o: test_3d.cpp plotter.h plotter_core.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

test_points.o: test_points.cpp plotter.h plotter_core.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

test_core.o: test_core.cpp plotter_core.h
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f *.o *.a test_2d test_3d test_points test_core
