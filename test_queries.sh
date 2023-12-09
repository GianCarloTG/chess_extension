echo TESTING PROJECT
sh install_chess_library.sh
sleep 1
docker exec -it psql_chess psql -h localhost -U gian -c "CREATE DATABASE dbsa_project;"
sleep 1
docker exec -it psql_chess psql -h localhost -U gian -d dbsa_project -c "\i /home/sql/load_data.sql"
sleep 1
docker exec -it psql_chess psql -h localhost -U gian -d dbsa_project -c "\i /home/sql/functions_indexes.sql"
# sleep 1
# docker exec -it psql_chess psql -h localhost -U gian -d dbsa_project -c "\di+ btree_has_opening"
