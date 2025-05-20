FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    build-essential \
    g++ \
    flex \
    bison \
    byacc \
    sqlite3 \
    libsqlite3-dev \
    make \
    curl \
    git \
    bash && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /home/minisql

COPY . .

RUN cd code && make init && make all

CMD ["/home/minisql/execs/main.out"]
