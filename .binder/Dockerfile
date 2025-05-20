# =============================
# Stage 1: Compile Everything
# =============================
FROM ubuntu:22.04 AS builder

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

# Build the project
RUN cd code && make init && make all

# =============================
# Stage 2: Runtime Only
# =============================
FROM ubuntu:22.04

RUN apt-get update -y && apt-get install -y \
    sqlite3 \
    libsqlite3-0 \
    bash \
 && apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /home/minisql

# Copy only executables and required metadata/data
COPY --from=builder /home/minisql/code/execs ./execs
COPY --from=builder /home/minisql/code/tpch ./tpch
COPY --from=builder /home/minisql/code/metadata ./metadata
COPY --from=builder /home/minisql/launch.sh ./launch.sh

# Make launch.sh executable
RUN chmod +x launch.sh

# Default command
CMD ["./launch.sh"]
