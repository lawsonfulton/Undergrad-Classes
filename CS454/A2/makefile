all: stringClient stringServer

stringClient: stringClient.cpp helperFcns.h
	g++ -pthread -o stringClient stringClient.cpp

stringServer: stringServer.cpp helperFcns.h
	g++ -pthread -o stringServer stringServer.cpp

