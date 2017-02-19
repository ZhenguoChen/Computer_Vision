/*
 subscribe camera frams and do blob detection
 */

#include <ros/ros.h>
#include <image_transport/image_transport.h>
#include <opencv2/highgui.hpp>
#include <opencv2/opencv.hpp>
#include <cv_bridge/cv_bridge.h>

using namespace cv;
using namespace std;

SimpleBlobDetector::Params params;
Ptr<SimpleBlobDetector> detector;

bool init_params(Mat im)
{
    //set blob detector parameter
    //set threshold
    params.minThreshold = 10;
    params.maxThreshold = 250;

    //Filter by area
    params.filterByArea = true;
    params.minArea = im.cols*im.rows/1000;
    params.maxArea = im.cols*im.rows/2;

    //filter by circularity
    params.filterByCircularity = true;
    params.minCircularity = 0.1;

    //filter by convexity
    params.filterByConvexity = true;
    params.minConvexity = 0.87;

    //filter by Inertia
    params.filterByInertia = true;
    params.minInertiaRatio = 0.01;
}

void blob_detect(const sensor_msgs::ImageConstPtr& msg){
    
    try{
        //convert image
        Mat im;
        im = cv_bridge::toCvShare(msg, "bgr8")->image;
        static bool init_FLAG = init_params(im);

        if(im.empty()){
            cout<<"failed to convert image"<<endl;
            return;
        }
        //detect blobs
        vector<KeyPoint> keypoints;
        vector<KeyPoint> keypoints2;

        //detect dark blobs
        params.blobColor = 0;
        detector = SimpleBlobDetector::create(params);
        detector->detect(im, keypoints);

        //detect light blobs
        params.blobColor = 255;
        detector = SimpleBlobDetector::create(params);
        detector->detect(im, keypoints2);

        keypoints.insert(keypoints.begin(), keypoints2.begin(), keypoints2.end());

        Mat im_with_keypoints;
        try{
            drawKeypoints(im, keypoints, im_with_keypoints, Scalar(0, 0, 255), DrawMatchesFlags::DRAW_RICH_KEYPOINTS);
        }
        catch(Exception e){
            cout<<e.msg<<endl;
        }

        imshow("video", im_with_keypoints);
        //must call waitKey so that the window can be updated
        waitKey(30);
    }
    catch(cv_bridge::Exception& e){
        cout<<"failed to convert image"<<endl;
    }
}

int main(int argc, char **argv)
{
    ros::init(argc, argv, "blob_detector");
    ros::NodeHandle n;
    namedWindow("video");
    startWindowThread();

    image_transport::ImageTransport it(n);
    image_transport::Subscriber sub = it.subscribe("camera/image", 1, blob_detect);
    ros::spin();
    destroyWindow("video");
}
