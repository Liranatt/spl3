# Makefile for the Stomp Client Project

CFLAGS := -c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS := -lboost_system -lpthread

# Please implement your Makefile rules and targets below.
# Customize this file to define how to build your project.

# all: clean link

# run: link
# 	valgrind --leak-check=full --show-reachable=yes -s bin/main config_file.txt

# link: compile
# 	g++ -o bin/simulation bin/main.o bin/Settlement.o bin/Facility.o bin/SelectionPolicy.o bin/Plan.o bin/Auxiliary.o bin/Simulation.o bin/Action.o

# compile: src/main.cpp src/Settlement.cpp src/Facility.cpp src/SelectionPolicy.cpp src/Plan.cpp src/Auxiliary.cpp src/Simulation.cpp src/Action.cpp
# 	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp
# 	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp
# 	g++ $(CFLAGS) -o bin/event.o src/event.cpp
# 	g++ $(CFLAGS) -o bin/FrameCodec.o src/FrameCodec.cpp
# 	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp
# 	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp


# clean:
# 	clear
# 	rm -f bin/*

CXX = g++
CXXFLAGS = -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS = -lboost_system -lpthread
OBJ_DIR = bin
SRC_DIR = src
INCLUDE_DIR = include

SOURCES = $(SRC_DIR)/echoClient.cpp \
          $(SRC_DIR)/ConnectionHandler.cpp \
          $(SRC_DIR)/StompClient.cpp \
          $(SRC_DIR)/StompProtocol.cpp \
          $(SRC_DIR)/FrameCodec.cpp \
          $(SRC_DIR)/event.cpp
          
OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))

all: $(OBJ_DIR)/EchoClient $(OBJ_DIR)/StompWCIClient

$(OBJ_DIR)/EchoClient: $(OBJ_DIR)/ConnectionHandler.o $(OBJ_DIR)/echoClient.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/StompWCIClient: $(OBJ_DIR)/ConnectionHandler.o $(OBJ_DIR)/StompClient.o $(OBJ_DIR)/StompProtocol.o $(OBJ_DIR)/FrameCodec.o $(OBJ_DIR)/event.o
	$(CXX) -o $@ $^ $(LDFLAGS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CXX) $(CXXFLAGS) -c $< -o $@

clean:
	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/EchoClient $(OBJ_DIR)/StompWCIClient


# Targets
# all: EchoClient StompWCIClient

# EchoClient: bin/ConnectionHandler.o bin/echoClient.o
# 	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)

# StompWCIClient: bin/ConnectionHandler.o bin/StompClient.o bin/event.o
# 	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompClient.o bin/event.o $(LDFLAGS)

# bin/ConnectionHandler.o: src/ConnectionHandler.cpp include/ConnectionHandler.h
# 	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp

# bin/echoClient.o: src/echoClient.cpp include/ConnectionHandler.h
# 	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp

# bin/event.o: src/event.cpp include/event.h
# 	g++ $(CFLAGS) -o bin/event.o src/event.cpp

# bin/StompClient.o: src/StompClient.cpp include/StompProtocol.h include/ConnectionHandler.h
# 	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp

# .PHONY: clean
# clean:
# 	rm -f bin/*
