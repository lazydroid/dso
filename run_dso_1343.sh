#!/bin/bash

# start=1499 for reverse

~/dso/build/bin/dso_dataset \
files=~/Downloads/zed_2019-12-05_134328/color \
calib=~/Downloads/zed_2019-12-05_134328/camera.txt \
gamma=~/Downloads/zed_2019-12-05_134328/pcalib.txt \
vignette=~/Downloads/zed_2019-12-05_134328/vignette.png \
reverse=0 \
preset=0 \
start=300 \
mode=1
