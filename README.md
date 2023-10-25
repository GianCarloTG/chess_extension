# Docker
To run the docker run 
```
docker build -t complex:Dockerfile --progress=plain --no-cache . &> build.log
```

now to run it you use 
```
docker run --name complex --rm -e POSTGRES_PASSWORD=gabriel -e POSTGRES_USER=gabriel -p 5432:5432 complex:Dockerfile
```
