#!/bin/sh

docker cp complex-template complex:/home/
docker cp install_in_docker.sh complex:/home/
docker exec -it complex /bin/sh /home/install_in_docker.sh