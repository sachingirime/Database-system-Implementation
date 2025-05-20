# ---------- Stage 1: Build the executable ----------
FROM ubuntu:22.04 AS builder

RUN apt-get update && apt-get install -y \
    build-essential g++ flex bison byacc \
    sqlite3 libsqlite3-dev make curl git

WORKDIR /home/minisql

# Copy everything and build
COPY . .
RUN cd code && make init && make all


# ---------- Stage 2: Runtime environment ----------
FROM ubuntu:22.04

RUN apt-get update && apt-get install -y \
    sqlite3 libsqlite3-0 python3 python3-pip && \
    apt-get clean && rm -rf /var/lib/apt/lists/*

WORKDIR /home/minisql

# Copy final built binary and API server
COPY --from=builder /home/minisql/execs ./execs
COPY web_api.py ./web_api.py

# Install FastAPI and Uvicorn
RUN pip3 install fastapi uvicorn

# Expose port for Uvicorn server
EXPOSE 8000

# Run the FastAPI app
CMD ["uvicorn", "web_api:app", "--host", "0.0.0.0", "--port", "8000"]
