GCR_HOST = gcr.io
PROJECT_ID = sigma-scheduler-405523
REPO_NAME = be-api
TAG = latest

IMAGE = $(GCR_HOST)/$(PROJECT_ID)/$(REPO_NAME):${TAG}

CC = g++
CXX = clang++
CXXFLAGS = -std=c++17 -Wall -o2 -Wextra -Iinclude -pedantic -g -I/usr/local/include -I./include/asio-1.28.0/include -Isrc/nlohmann
LFLAGS = -lpqxx -lboost_system -lpthread
INCLUDES = -I./include/asio-1.28.0/include

deploy-latest: build tag push

all: api

api: src/main.cpp src/db.cpp src/routes.cpp src/parser.cpp src/chain_utils.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -o zcash-api src/main.cpp src/db.cpp src/routes.cpp src/parser.cpp src/chain_utils.cpp $(LFLAGS)

clean:
	rm -f zcash-api

build: 
	docker build -t $(IMAGE) .

tag:
	docker tag $(IMAGE) $(IMAGE)

push:
	docker push $(IMAGE)

run:
	docker run $(IMAGE)

docker-run:
	docker run -p 8000:8000 -e PORT=8000 $(MAGE)
