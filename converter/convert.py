import constants
import math
import json
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
    print(f"---------- processing {name} ----------")

    # read tif
    print("reading tif")
    dataset = gdal.Open(f"{constants.DATA_DIR}/tiffs/{filename}", gdal.GA_ReadOnly)
    geotransform = dataset.GetGeoTransform()
    band = dataset.GetRasterBand(1)
    values = band.ReadAsArray()
    dataset.Close()

    # compute spatial information
    print("computing metadata")
    width, height = values.shape
    top_left = [geotransform[0], geotransform[3]]
    bottom_right = [
        geotransform[0] + width * geotransform[1] + height * geotransform[2],
        geotransform[3] + width * geotransform[4] + height * geotransform[5]
    ]
    metadata = {
        "min": [top_left[0], bottom_right[1]],
        "max": [bottom_right[0], top_left[1]]
    }
    with open(f"{constants.DATA_DIR}/terrarium/{name}.json", "w") as f:
        json.dump(metadata, f, indent=4)

    # encode image
    print("encoding to terrarium")
    terrarium = encode_terrarium_array(values)

    print("saving encoded image")
    img = Image.fromarray(terrarium)
    img.save(f"{constants.DATA_DIR}/terrarium/{name}.png")

    print(f"----------{"-" * len(f" processing {name}")}-----------")

gdal.UseExceptions()
for filename in os.listdir(f"{constants.DATA_DIR}/tiffs/"):
    convert(filename)