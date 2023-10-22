CXX = g++
CXXFLAGS = -Wall -g
IODIR =../../proj0_IO/

proj0: fuel.o mytest.cpp
	$(CXX) $(CXXFLAGS) fuel.o mytest.cpp -o proj0

fuel.o: fuel.h fuel.cpp
	$(CXX) $(CXXFLAGS) -c fuel.cpp

clean:
	rm *.o*
	rm *~

run:
	./proj0

val:
	valgrind ./proj0