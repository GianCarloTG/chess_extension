# Docker
To run the docker run 
```
docker build -t psql_chess:Dockerfile --progress=plain --no-cache . &> build.log
```

now to run it you use 
```
docker run --name psql_chess --rm -e POSTGRES_PASSWORD=gian -e POSTGRES_USER=gian -p 5434:5432 psql_chess:Dockerfile
```

install complex extension (for testing)

```
sh install_complex_library.sh
```

