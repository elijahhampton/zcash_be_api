GCR_HOST = gcr.io
PROJECT_ID = sigma-scheduler-405523
REPO_NAME = be-api
IMAGE_TAG = development

IMAGE = $(GCR_HOST)/$(PROJECT_ID)/$(REPO_NAME):$(IMAGE_TAG)

CC = g++
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -Wextra -Iinclude -pedantic -g -I/usr/local/include -I./include/asio-1.28.0/include -Isrc/nlohmann
LFLAGS = -lpqxx -lboost_system -lpthread
INCLUDES = -I./include/asio-1.28.0/include

all: api

api: src/main.cpp src/db.cpp src/routes.cpp src/parser.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o zcash-api src/main.cpp src/db.cpp src/routes.cpp src/parser.cpp $(LFLAGS)
 
clean:
	rm -f zcash-api

build:
	docker build -t $(REPO_NAME) .

tag:
	docker tag $(REPO_NAME) $(IMAGE)

push:
	docker push $(IMAGE)