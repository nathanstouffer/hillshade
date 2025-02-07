FROM ghcr.io/osgeo/gdal:ubuntu-small-3.10.1

RUN apt-get update
RUN apt-get install -y python3-full python3-pip python3-venv

WORKDIR /src

RUN apt-get install -y python3-gdal
RUN apt-get install -y python3-pil