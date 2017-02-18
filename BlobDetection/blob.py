#!/usr/bin/python

# Standard imports
import cv2
import numpy as np;

# Read image
im = cv2.imread("blob.jpg", cv2.IMREAD_UNCHANGED)

if(len(im)):
	print "failed reading image"

# Setup SimpleBlobDetector parameters.
params = cv2.SimpleBlobDetector_Params()

# 0 to extract dark blobs
# 255 to extract light blobs
#params.blobColor = 255

# Change thresholds
params.minThreshold = 10
params.maxThreshold = 250


# Filter by Area.
params.filterByArea = True
params.minArea = im.shape[0]*im.shape[1]/1000
params.maxArea = im.shape[0]*im.shape[1]/2

# Filter by Circularity
params.filterByCircularity = True
params.minCircularity = 0.1

# Filter by Convexity
params.filterByConvexity = True
params.minConvexity = 0.87
    
# Filter by Inertia
params.filterByInertia = True
params.minInertiaRatio = 0.01

# Create a detector with the parameters
ver = (cv2.__version__).split('.')
if int(ver[0]) < 3 :
	#detect dark blobs
	params.blobColor = 0
	detector = cv2.SimpleBlobDetector(params)
	keypoints = detector.detect(im)
	#detect light blobs
	params.blobColor = 255
	detector = cv2.SimpleBoloDetector(params)
	keypoints+=detector.detect(im)
else : 
	#detect dark blobs
	params.blobColor = 0
	detector = cv2.SimpleBlobDetector_create(params)
	keypoints = detector.detect(im)
	#detect light blobs
	params.blobColor = 255
	detector = cv2.SimpleBlobDetector_create(params)
	keypoints+=detector.detect(im)

# Detect blobs.
#keypoints = detector.detect(im)

# Draw detected blobs as red circles.
# cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS ensures
# the size of the circle corresponds to the size of blob

im_with_keypoints = cv2.drawKeypoints(im, keypoints, np.array([]), (0,0,255), cv2.DRAW_MATCHES_FLAGS_DRAW_RICH_KEYPOINTS)

# Show blobs
cv2.imshow("Keypoints", im_with_keypoints)
cv2.waitKey(0)

