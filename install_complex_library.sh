#!/bin/sh

docker cp complex-template psql_chess:/home/
docker cp install_in_docker.sh psql_chess:/home/
docker exec -it psql_chess /bin/sh /home/install_in_docker.sh