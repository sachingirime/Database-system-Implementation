FROM ubuntu:22.04

# Install dependencies
RUN apt-get update -y && apt-get upgrade -y && apt-get install -y \
    curl make bison gcc g++ yacc flex sqlite3 libsqlite3-dev git

# Set working directory inside container
WORKDIR /home/minisql

# Copy all project files into container
COPY . .

# Build the project
RUN cd code && make init && make main.out

# Default command on container start
CMD ["/bin/bash"]
