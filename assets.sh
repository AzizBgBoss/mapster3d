#!/bin/sh

NITRO_ENGINE=~/nitro-engine
ASSETS=assets
TOOLS=$NITRO_ENGINE/tools
OBJ2DL=$TOOLS/obj2dl/obj2dl.py

mkdir -p data

for obj in "$ASSETS"/*.obj; do
    [ -e "$obj" ] || continue
    name=$(basename "$obj" .obj)

    python3 $OBJ2DL \
        --input "$obj" \
        --output "data/$name.bin" \
        --texture 256 256 \
        --scale 1
done