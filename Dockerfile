FROM python:3.8-slim-buster

WORKDIR /app

COPY requirements.txt requirements.txt
RUN pip3 install --upgrade pip -r requirements.txt

COPY . .

EXPOSE 9658

CMD [ "python3", "./app.py" ]