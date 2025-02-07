import constants
import os
from osgeo import gdal

def convert(filename: str):
    name = filename.split('.')[0]
    print(name)
    dataset = gdal.Open(f"{constants.DATA_DIR}/tiffs/{filename}", gdal.GA_ReadOnly)
    band = dataset.GetRasterBand(1)
    arr = band.ReadAsArray()
    print(arr.shape)
    dataset.Close()

gdal.UseExceptions()
for filename in os.listdir(f"{constants.DATA_DIR}/tiffs/"):
    convert(filename)