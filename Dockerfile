FROM python:3.8-slim-buster

WORKDIR /app

COPY requirements.txt requirements.txt
RUN pip3 install --upgrade pip -r requirements.txt

COPY . .

RUN apt-get update && apt-get -y install cmake protobuf-compiler

RUN cd lib && ccmake ../feed_uvl_finding_comparatively && make

RUN cd .. && ls

EXPOSE 9658
CMD [ "python3", "./app.py" ]