#!/bin/sh

NITRO_ENGINE=~/nitro-engine
ASSETS=assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py

rm -rf data
mkdir -p data

python3 $OBJ2DL \
    --input $ASSETS/cube.obj \
    --output data/cube.bin \
    --texture 256 256
