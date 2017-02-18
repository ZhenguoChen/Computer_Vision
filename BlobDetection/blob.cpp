/**
 * OpenCV SimpleBlobDetector Example
 *
 * Copyright 2015 by Satya Mallick <spmallick@gmail.com>
 *
 */

#include "opencv2/opencv.hpp"

using namespace cv;
using namespace std;

int main( int argc, char** argv )
{
	Mat im;
	// Read image
	try{
		//im = imread( "blob.jpg", IMREAD_UNCHANGED );
		im = imread( "images.png", IMREAD_UNCHANGED);
	}catch(Exception e){
		cout<< e.msg<<endl;
	}

	if(im.empty()){
		cout<<"failed reading image"<<endl;
		return -1;
	}

	// Setup SimpleBlobDetector parameters.
	SimpleBlobDetector::Params params;

	// 0 to extract dark blobs
	// 255 to extract light blobs
	//params.filterByColor = true;
    //params.blobColor = 255;

	// Change threshold, pixel smaller than minThreshold and larger than maxThreshold set to zero
	params.minThreshold = 10;
	params.maxThreshold = 250;

	// Filter by Area.
	params.filterByArea = true;
	params.minArea = im.cols*im.rows/1000;
	params.maxArea = im.cols*im.rows/2;

	// Filter by Circularity
	params.filterByCircularity = true;
	params.minCircularity = 0.1;

	// Filter by Convexity
	params.filterByConvexity = true;
	params.minConvexity = 0.87;

	// Filter by Inertia
	params.filterByInertia = true;
	params.minInertiaRatio = 0.01;

	// Storage for blobs
	vector<KeyPoint> keypoints;


#if CV_MAJOR_VERSION < 3   // If you are using OpenCV 2

	// Set up detector with params
	SimpleBlobDetector detector(params);

	// Detect blobs
	detector.detect( im, keypoints);
#else 

	// dark blob detection
    params.blobColor = 0;
	Ptr<SimpleBlobDetector> detector = SimpleBlobDetector::create(params);   
	// Detect blobs
	detector->detect( im, keypoints);

	// light blob detection
	vector<KeyPoint> keypoints2;
	params.blobColor = 255;
	detector = SimpleBlobDetector::create(params);
	detector->detect(im, keypoints2);
	keypoints.insert(keypoints.end(), keypoints2.begin(), keypoints2.end());
#endif

	// Draw detected blobs as red circles.
	// DrawMatchesFlags::DRAW_RICH_KEYPOINTS flag ensures
	// the size of the circle corresponds to the size of blob

	Mat im_with_keypoints;
	try{
		drawKeypoints( im, keypoints, im_with_keypoints, Scalar(0,0,255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS );
	}
    catch(Exception e){
		cout<<e.msg<<endl;
	}

	// Show blobs
	imshow("keypoints", im_with_keypoints );
	waitKey(0);

}

