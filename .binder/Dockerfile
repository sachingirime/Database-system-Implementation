# --- Stage 1: Builder ---
FROM ubuntu:22.04 as builder

RUN apt-get update && apt-get install -y \
    build-essential g++ flex bison byacc sqlite3 libsqlite3-dev make curl git

WORKDIR /home/minisql
COPY . .

RUN cd code && make init && make all

# --- Stage 2: Runtime ---
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    sqlite3 libsqlite3-0 bash && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /home/minisql

# Only copy final executables
COPY --from=builder /home/minisql/execs /home/minisql/execs

# Optional: run main CLI directly
CMD ["/home/minisql/execs/main.out"]
