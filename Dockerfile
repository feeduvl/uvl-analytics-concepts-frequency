FROM debian:latest

RUN apt update && apt install -y build-essential cmake
RUN add-apt-repository ppa:ubuntu-toolchain-r/test
RUN apt update
RUN apt install gcc-9 g++-9
RUN gcc --version
WORKDIR /app

COPY . .

WORKDIR lib

RUN cmake -S . -B . && make

RUN echo "Result after CMake build: " && ls

RUN apt install python3 python3-pip && rm -rf /var/lib/apt/lists/*

COPY requirements.txt requirements.txt
RUN pip3 install --upgrade pip -r requirements.txt

WORKDIR ..
EXPOSE 9658
CMD [ "python3", "./app.py" ]