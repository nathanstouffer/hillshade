# FROM python:3.11-slim

# RUN apt-get update
# RUN apt list -a gdal-bin libgdal-dev
# RUN apt-get install -y libgdal-dev=3.10

FROM ghcr.io/osgeo/gdal:ubuntu-small-3.10.1

RUN apt-get update
RUN apt-get install -y python3-full python3-pip python3-venv

# RUN python3 -m venv /venv
# RUN /bin/bash -c "source /venv/bin/activate"

WORKDIR /src

# COPY converter/requirements.txt .

# RUN pip3 install --no-cache-dir -r requirements.txt

RUN apt-get install -y python3-gdal