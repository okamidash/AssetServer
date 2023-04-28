FROM ubuntu:22.10

RUN echo "establishing"

RUN apt update && apt install -y build-essential

RUN echo "start"

RUN mkdir -p /srv/src/

COPY Makefile /srv/src/  
COPY *.c /srv/src/  
COPY *.h /srv/src/  

WORKDIR /srv/src

ENV SERVER="./server"

RUN make
CMD ${SERVER}

RUN echo "end"
