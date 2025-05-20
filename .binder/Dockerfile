FROM ubuntu:22.04

RUN apt-get update -y && apt-get install -y \
    build-essential \
    g++ \
    flex \
    bison \
    byacc \
    sqlite3 \
    libsqlite3-dev \
    make \
    curl \
    git

WORKDIR /home/minisql

COPY . .

RUN cd code && make init && make all

CMD ["/bin/bash"]
