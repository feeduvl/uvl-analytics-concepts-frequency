FROM python:3.8-slim-buster

WORKDIR /app

COPY requirements.txt requirements.txt
RUN pip3 install --upgrade pip -r requirements.txt

COPY . .

RUN apt-get update && yes | apt-get install build-essential
RUN apt-get -y install cmake protobuf-compiler

RUN cd ./lib/

RUN cmake && make

RUN echo "Result after CMake build: " && ls

EXPOSE 9658
CMD [ "python3", "./app.py" ]