# Docker
Project developed by:
* Jakub Kwiatkowski
* Gabriela Kaczmarek
* Dilbar Isakova
* Gian Carlo Tejada Gargate

To run the docker run 
```
docker build -t psql_chess:Dockerfile --progress=plain --no-cache . &> build.log
```

now to run it, you use 
```
docker run --name psql_chess --rm -e POSTGRES_PASSWORD=gian -e POSTGRES_USER=gian -p 5434:5432 psql_chess:Dockerfile
```

install and test chess extension (the sql files for testing are located in the sql folder)

```
sh test_queries.sh
```

