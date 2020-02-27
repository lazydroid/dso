#!/bin/bash

~/dso/build/bin/dso_dataset \
files=~/Downloads/vision.in.tum.de/sequence_$1/images.zip \
calib=~/Downloads/vision.in.tum.de/sequence_$1/camera.txt \
gamma=~/Downloads/vision.in.tum.de/sequence_$1/pcalib.txt \
vignette=~/Downloads/vision.in.tum.de/sequence_$1/vignette.png \
preset=0 \
realsense=0
mode=0
