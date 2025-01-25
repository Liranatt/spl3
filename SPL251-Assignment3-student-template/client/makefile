# Makefile for the Stomp Client Project

CFLAGS := -c -Wall -Weffc++ -g -std=c++11 -Iinclude
LDFLAGS := -lboost_system -lpthread

all: clean run

StompClient: 
	run

run: bin/StompClient.o
	bin/StompClient 127.0.0.1 7777

bin/StompClient.o: compile
	g++ -o bin/StompClient bin/ConnectionHandler.o  bin/event.o bin/FrameCodec.o bin/StompClient.o bin/StompProtocol.o $(LDFLAGS)

compile: src/ConnectionHandler.cpp src/echoClient.cpp src/event.cpp src/FrameCodec.cpp src/StompClient.cpp src/StompProtocol.cpp
	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp
	g++ $(CFLAGS) -o bin/event.o src/event.cpp
	g++ $(CFLAGS) -o bin/FrameCodec.o src/FrameCodec.cpp
	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp
	g++ $(CFLAGS) -o bin/StompProtocol.o src/StompProtocol.cpp

.PHONY: clean
clean:
	clear
	rm -f bin/*


# 	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp
# bin/echoClient.o

############################################# chatgpt template
# CXX = g++
# CXXFLAGS = -Wall -Weffc++ -g -std=c++11 -Iinclude
# LDFLAGS = -lboost_system -lpthread
# OBJ_DIR = bin
# SRC_DIR = src
# INCLUDE_DIR = include
#
# SOURCES = $(SRC_DIR)/echoClient.cpp \
#           $(SRC_DIR)/ConnectionHandler.cpp \
#           $(SRC_DIR)/StompClient.cpp \
#           $(SRC_DIR)/StompProtocol.cpp \
#           $(SRC_DIR)/FrameCodec.cpp \
#           $(SRC_DIR)/event.cpp
#
# OBJECTS = $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SOURCES))
#
# all: $(OBJ_DIR)/EchoClient $(OBJ_DIR)/StompWCIClient
#
# $(OBJ_DIR)/EchoClient: $(OBJ_DIR)/ConnectionHandler.o $(OBJ_DIR)/echoClient.o
# 	$(CXX) -o $@ $^ $(LDFLAGS)
#
# $(OBJ_DIR)/StompWCIClient: $(OBJ_DIR)/ConnectionHandler.o $(OBJ_DIR)/StompClient.o $(OBJ_DIR)/StompProtocol.o $(OBJ_DIR)/FrameCodec.o $(OBJ_DIR)/event.o
# 	$(CXX) -o $@ $^ $(LDFLAGS)
#
# $(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
# 	$(CXX) $(CXXFLAGS) -c $< -o $@
#
# clean:
# 	rm -f $(OBJ_DIR)/*.o $(OBJ_DIR)/EchoClient $(OBJ_DIR)/StompWCIClient

############################################# original template
# TODO change the makefile for StompEMIlient
# all: EchoClient
#
# EchoClient: bin/ConnectionHandler.o bin/echoClient.o
# 	g++ -o bin/EchoClient bin/ConnectionHandler.o bin/echoClient.o $(LDFLAGS)
#
# StompWCIClient: bin/ConnectionHandler.o bin/StompClient.obin/event.o
# 	g++ -o bin/StompWCIClient bin/ConnectionHandler.o bin/StompClient.o $(LDFLAGS)
#
# bin/ConnectionHandler.o: src/ConnectionHandler.cpp
# 	g++ $(CFLAGS) -o bin/ConnectionHandler.o src/ConnectionHandler.cpp
#
# bin/echoClient.o: src/echoClient.cpp
# 	g++ $(CFLAGS) -o bin/echoClient.o src/echoClient.cpp
#
# bin/event.o: src/event.cpp
# 	g++ $(CFLAGS) -o bin/event.o src/event.cpp
#
# bin/StompClient.o: src/StompClient.cpp
# 	g++ $(CFLAGS) -o bin/StompClient.o src/StompClient.cpp
#
# .PHONY: clean
# clean:
# 	rm -f bin/*

############################################# michael template

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
