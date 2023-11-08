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

ENV DB_HOST=db \
    DB_PORT=5432 \
    DB_NAME=postgres \
    DB_USER=postgres \
    DB_PASSWORD=mysecretpassword

WORKDIR /blockexplorer-api

COPY . .

RUN CXX=clang++ make api

EXPOSE 5000

CMD ["./zcash-api"]
