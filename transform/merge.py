import argparse
import constants
from osgeo import gdal


def merge(args: argparse.Namespace):
    src1 = f"{constants.DATA_DIR}/{args.src1}"
    src2 = f"{constants.DATA_DIR}/{args.src2}"
    trg = f"{constants.DATA_DIR}/{args.trg}"

    print(f"---------- merging {args.src1} + {args.src2} ----------")

    # Open the input files
    dataset1 = gdal.Open(src1, gdal.GA_ReadOnly)
    dataset2 = gdal.Open(src2, gdal.GA_ReadOnly)

    if dataset1 is None:
        raise RuntimeError(f"Could not open {src1}")

    if dataset2 is None:
        raise RuntimeError(f"Could not open {src2}")

    # Merge the two rasters into one
    gdal.Warp(
        trg,
        [dataset1, dataset2],
        format="GTiff"
    )

    dataset1.Close()
    dataset2.Close()

    print(f"---------- merged into {args.trg} ----------")


gdal.UseExceptions()

parser = argparse.ArgumentParser()

parser.add_argument(
    'src1',
    help='The (relative) path of the first input file'
)

parser.add_argument(
    'src2',
    help='The (relative) path of the second input file'
)

parser.add_argument(
    'trg',
    help='The (relative) path of the output file'
)

args = parser.parse_args()

merge(args)
