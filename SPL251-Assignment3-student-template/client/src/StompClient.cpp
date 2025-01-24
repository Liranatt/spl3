#include <iostream>
#include <string>
#include "../include/ConnectionHandler.h"



int main(int argc, char *argv[]) {
	if (argc < 3){
		std::cerr << "Usage: " << argv[0] << " <host> <port>" <<std::endl;
		return -1;
	}
	std::string host = argv[i];
	short port = std::stoi(argv[2]);
	ConnectionHandler ConnectionHandler(host,port);
	if(!ConnectionHandler.connect()){
		std::cerr << "Cannot connect to " <<host << ":" << port << std::endl;
		return -1;
	}
	std::string connectFrame = stompProtocol.create
	return 0;
}