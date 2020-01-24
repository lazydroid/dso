#!/usr/bin/env python

import urllib
import os, sys

URL = 'http://vision.in.tum.de/mono/dataset/sequence_%02d.zip'

def download_progress(count, blockSize, totalSize):
    #print(count, blockSize, totalSize)
	print '%.02f %%  \r' % (count * blockSize * 100.0 / totalSize),
	sys.stdout.flush()

for i in range(1,18) :
	url = URL % i
	name = url.split('/')[-1]
	target = '../Downloads/vision.in.tum.de/' + name

	if os.path.isfile( target ) :
		print target, 'already downloaded'
	else :
		print target, 'downloading...'

		urllib.urlretrieve( url, target, download_progress )

