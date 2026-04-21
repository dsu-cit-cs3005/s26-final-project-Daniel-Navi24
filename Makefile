# Compiler
CXX = g++
CXXFLAGS = -std=c++20 -Wall -Wextra -pedantic
PICFLAGS = -fPIC

# Targets
all: test_robot RobotWarz

# RobotBase.o needs to be compiled with -fPIC for use in shared libraries
RobotBase.o: RobotBase.cpp RobotBase.h
	$(CXX) $(CXXFLAGS) $(PICFLAGS) -c RobotBase.cpp -o RobotBase.o

test_robot: test_robot.cpp RobotBase.o
	$(CXX) $(CXXFLAGS) test_robot.cpp RobotBase.o -ldl -o test_robot

RobotWarz: RobotWarz.cpp Arena.cpp RobotBase.o
	$(CXX) $(CXXFLAGS) RobotWarz.cpp Arena.cpp RobotBase.o -ldl -o RobotWarz

clean:
	rm -f *.o test_robot *.so RobotWarz
