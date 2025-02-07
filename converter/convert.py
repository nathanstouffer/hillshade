import constants
import math
import numpy as np
import os
from osgeo import gdal
from PIL import Image

def encode_terrarium_value(z: float):
    z += 32768
    r = math.floor(z / 256)
    g = math.floor(z % 256)
    b = math.floor((z - math.floor(z)) * 256)
    return (r, g, b)

def encode_terrarium_array(array: np.ndarray):
    encoded = array.squeeze() + 32768
    r = np.floor(encoded / 256.0)
    g = np.floor(np.mod(encoded, 256.0))
    b = np.floor((encoded - np.floor(encoded)) * 256.0)
    return np.stack([r, g, b], axis=2).astype(np.uint8)

def convert(filename: str):
    name = filename.split('.')[0]
    print(f"processing {name}")

    # read tif
    dataset = gdal.Open(f"{constants.DATA_DIR}/tiffs/{filename}", gdal.GA_ReadOnly)
    band = dataset.GetRasterBand(1)
    values = band.ReadAsArray()
    dataset.Close()

    terrarium = encode_terrarium_array(values)

    img = Image.fromarray(terrarium)
    img.save(f"{constants.DATA_DIR}/terrarium/{name}.png")

gdal.UseExceptions()
for filename in os.listdir(f"{constants.DATA_DIR}/tiffs/"):
    convert(filename)