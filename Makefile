CC = g++
CXX = g++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -pedantic -g -I/usr/local/include
LFLAGS = -lpqxx -lboost_system
INCLUDES = -I./include/asio-1.28.0/include

all: api

api: src/main.cpp src/db.cpp src/routes.cpp src/parser.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o zcash-api src/main.cpp src/db.cpp src/routes.cpp src/parser.cpp $(LFLAGS)
 
clean:
	rm -f zcash-api
