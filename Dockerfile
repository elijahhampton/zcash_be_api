FROM ubuntu:20.04

# Avoid interactive dialogue with tzdata
ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y \
    clang \
    g++ \
    make \
    libpqxx-dev \
    libboost-system-dev \
    git

# Set other DB environment variables
ENV DB_PORT=5432 \
    DB_NAME=postgres \
    DB_USER=postgres \
    DB_HOST=host.docker.internal \
    DB_PASSWORD=password
    
# Note: DB_PASSWORD is not set here
# Note: DB_HOST is not set here

WORKDIR /blockexplorer-api

COPY . .

RUN CXX=clang++ make api

EXPOSE 5000

CMD ["./zcash-api"]
