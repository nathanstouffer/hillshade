FROM python:3.9-slim

WORKDIR /src

COPY converter/requirements.txt .

RUN pip install --no-cache-dir -r requirements.txt