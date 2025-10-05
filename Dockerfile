FROM debian:bullseye AS builder
# Here thr 'AS builder' means taht this stage is a build stage named 'builder'
#if i did not use 'AS builder' i would reference to this stage by its index '0'..

WORKDIR /app

RUN apt-get update && apt-get install -y build-essential make

COPY . .

RUN make
# the second stage

FROM debian:bullseye-slim
# i used slim 7itach it is smaller 30mb.

RUN apt-get update && apt-get install -y \
	php-cgi \
	python3 \
	&& rm -rf /var/lib/apt/lists/*
	#the last cmd deletes the packages that i dont need so the container stays small

WORKDIR /webserver

COPY ./configs ./configs/
COPY ./cgi-bin ./cgi-bin/
COPY ./www  ./www/

RUN chmod +x ./cgi-bin/*

COPY --from=builder /app/webserv .

EXPOSE 8080
CMD ["./webserv", "./configs/laaroubi.conf"]