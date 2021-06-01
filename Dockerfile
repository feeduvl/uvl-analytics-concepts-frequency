FROM debian:buster

RUN apt update && apt install -y gcc clang clang-tools cmake python3 python3-pip && rm -rf /var/lib/apt/lists/*

WORKDIR /app

COPY requirements.txt requirements.txt
RUN pip3 install --upgrade pip -r requirements.txt

COPY . .

WORKDIR lib

RUN cmake -S . -B . && make

RUN echo "Result after CMake build: " && ls

WORKDIR ..
EXPOSE 9658
CMD [ "python3", "./app.py" ]