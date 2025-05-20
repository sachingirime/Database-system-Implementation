FROM debian:bullseye-slim

# Install required tools
RUN apt-get update -y && apt-get install -y \
    build-essential \
    flex \
    bison \
    byacc \
    sqlite3 \
    libsqlite3-dev \
    curl \
    git \
    make

# Set working directory
WORKDIR /home/minisql

# Copy source code
COPY . .

# Build the project during image build to avoid runtime compile
RUN cd code && make init && make all

# Start with an interactive shell
CMD ["/bin/bash"]
