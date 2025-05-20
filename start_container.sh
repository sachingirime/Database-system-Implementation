# Build the docker image
docker build -t eecs277_image .

# Start the container and mount the course directory
docker run -it -v ~/2025-spring-cse177-eecs277:/root/2025-spring-cse177-eecs277 --name=project_container eecs277_image /bin/bash