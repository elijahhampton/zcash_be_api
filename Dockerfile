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

WORKDIR /blockexplorer-api

COPY . .

RUN CXX=clang++ make api

EXPOSE 8000

CMD ["./zcash-api"]
