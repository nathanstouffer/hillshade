import argparse
import constants
import os
from osgeo import gdal

def lerp(a, b, t):
    return a * (1 - t) + b * t

def cut(args: argparse.Namespace):
    name = args.src.split('/')[-1].split('.')[0]
    print(f"---------- processing {name} ----------")

    projWin = None
    if args.space_bounds:
        projWin = args.space_bounds
    elif args.image_bounds:
        dataset = gdal.Open(f"{constants.DATA_DIR}/{args.src}", gdal.GA_ReadOnly)
        geotransform = dataset.GetGeoTransform()
        width, height = dataset.RasterXSize, dataset.RasterYSize
        dataset.Close()
        ul = [geotransform[0], geotransform[3]]
        br = [
            geotransform[0] + width * geotransform[1] + height * geotransform[2],
            geotransform[3] + width * geotransform[4] + height * geotransform[5]
        ]
        projWin = [
            lerp(ul[0], br[0], args.image_bounds[0]),
            lerp(ul[1], br[1], args.image_bounds[1]),
            lerp(ul[0], br[0], args.image_bounds[2]),
            lerp(ul[1], br[1], args.image_bounds[3])
        ]
        print(projWin)
    else:
        print("Error: must specify bounds to cut")

    src = gdal.Open(f'{constants.DATA_DIR}/{args.src}')
    trg = gdal.Translate(f'{constants.DATA_DIR}/{args.trg}', src, projWin=projWin)
    src.Close()
    trg.Close()

    print(f"----------{"-" * len(f" processing {name}")}-----------")
    return

    # read tif
    print("reading tif")
    dataset = gdal.Open(f"{constants.DATA_DIR}//{filename}", gdal.GA_ReadOnly)
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

gdal.UseExceptions()

parser = argparse.ArgumentParser()
parser.add_argument('src', help='The (relative) path of the input file')
parser.add_argument('trg', help='The (relative) path of the output file')

group = parser.add_mutually_exclusive_group(required=True)
group.add_argument(
    '--space_bounds',
    type=float,
    nargs=4,
    help='The spatial bounds (lng/lat) used to clip the file (format: ulx uly lrx lry)'
)
group.add_argument(
    '--image_bounds',
    type=float,
    nargs=4,
    help='The image bounds (u/v) used to clip the file (format: ulx uly lrx lry)'
)

args = parser.parse_args()
cut(args)