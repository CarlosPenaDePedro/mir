#!/usr/bin/env python
import gdal
import sys
import json
import mmap

# Data at http://landcover.org/data/watermask/index.shtml

"""

Inspect file with gdalinfo

Driver: GTiff/GeoTIFF
Files: MOD44W_Water_2000_FE1718.tif
Size is 5761, 7681
Coordinate System is:
GEOGCS["WGS 84",
    DATUM["WGS_1984",
        SPHEROID["WGS 84",6378137,298.257223563,
            AUTHORITY["EPSG","7030"]],
        AUTHORITY["EPSG","6326"]],
    PRIMEM["Greenwich",0],
    UNIT["degree",0.0174532925199433],
    AUTHORITY["EPSG","4326"]]
Origin = (-84.000000000000000,-47.999997500000006)
Pixel Size = (0.002083333333333,-0.002083333043981)
Metadata:
  AREA_OR_POINT=Area
  TIFFTAG_XRESOLUTION=1
  TIFFTAG_YRESOLUTION=1
Image Structure Metadata:
  INTERLEAVE=BAND
Corner Coordinates:
Upper Left  ( -84.0000000, -47.9999975) ( 84d 0' 0.00"W, 47d59'59.99"S)
Lower Left  ( -84.0000000, -64.0020786) ( 84d 0' 0.00"W, 64d 0' 7.48"S)
Upper Right ( -71.9979167, -47.9999975) ( 71d59'52.50"W, 47d59'59.99"S)
Lower Right ( -71.9979167, -64.0020786) ( 71d59'52.50"W, 64d 0' 7.48"S)
Center      ( -77.9989583, -56.0010381) ( 77d59'56.25"W, 56d 0' 3.74"S)
Band 1 Block=5761x1 Type=Byte, ColorInterp=Gray
"""

"""
360/0.0020833333333333333 = 172800
180/0.0020833333333333333 = 86400


48 20160
-48 66240
-32 58560
-16 50880
0

"""


def s(x):
    return "%5g" % (x,)


header = "P5\n172800\n86400\n255\n"
g = open("p.pgm", "w+b")

line = bytearray([127 for y in xrange(0, 172800)])

g.write(header)
for r in xrange(0, 86400):
    g.write(line)Ï

# exit(0)

mapping = {0.0: 0, 1.0: 255, 253: 200}
X = {}
for path in sys.argv[1:]:
    print path
    raster = gdal.Open(path)
    (top_left_x, w_e_pixel_resolution, _, top_left_y, _, n_s_pixel_resolution) = raster.GetGeoTransform()

    # print "x=", top_left_x, " dx=", w_e_pixel_resolution
    # print "y=", top_left_y, " dy=", n_s_pixel_resolution
    x = raster.GetProjection()
    assert x == 'GEOGCS["WGS 84",DATUM["WGS_1984",SPHEROID["WGS 84",6378137,298.257223563,AUTHORITY["EPSG","7030"]],AUTHORITY["EPSG","6326"]],PRIMEM["Greenwich",0],UNIT["degree",0.0174532925199433],AUTHORITY["EPSG","4326"]]'

    # print dir(raster)
    assert raster.RasterCount == 1
    band = raster.GetRasterBand(1)
    assert band.GetScale() == 1
    values = band.ReadAsArray()
    (width, height) = values.shape
    print (width, height)

    while top_left_x >= 360:
        top_left_x -= 360

    while top_left_x < 0:
        top_left_x += 360

    col = int(top_left_x * 172800 / 360 + 0.5)
    row = int((90 - top_left_y) * 86400 / 180 + 0.5)

    print (col, row, col + width, row + height)
    print (top_left_x, top_left_y)

    for x in xrange(0, height):
        pos = len(header) + 172800 * (x + row-1) + col
        assert pos > 0
        g.seek(pos)
        p = [mapping[q] for q in values[x:x+1].flatten()]
        g.write(bytearray(p))

    # # print (width, height)
    # X.setdefault(s(top_left_x), {})
    # X[s(top_left_x)][s(top_left_y)] = (width,
    #                                    height,
    #                                    path,
    #                                    int(top_left_x * 172800 / 360 + 0.5),
    #                                    int((90 - top_left_y) * 86400 / 180 + 0.5),
    #                                    w_e_pixel_resolution,
    #                                    n_s_pixel_resolution,
    #                                    (top_left_x, top_left_y),
    #                                    (top_left_x + height * w_e_pixel_resolution,  top_left_y + width * n_s_pixel_resolution)
    #                                    )
    # print (top_left_x, top_left_y)

    # for y in xrange(0, width):
    #     lon = top_left_y + y * n_s_pixel_resolution
    #     for x in xrange(0, height):
    #         lat = top_left_x + x * w_e_pixel_resolution

    # print (lat, lon)

# print json.dumps(X, sort_keys=True, indent=4)
# for k, v in sorted(X.items()):
#     print k
#     print "     ", v
# print band.shape

# print "[ NO DATA VALUE ] = ", band.GetNoDataValue()
# print "[ MIN ] = ", band.GetMinimum()
# print "[ MAX ] = ", band.GetMaximum()
# print "[ SCALE ] = ", band.GetScale()
# print "[ UNIT TYPE ] = ", band.GetUnitType()
#print band.ReadAsArray()
