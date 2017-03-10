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

int red_range[6] = {17, 15, 100, 50, 56, 200};
int green_range[6] = {86, 31, 4, 220, 88, 50};
int blue_range[6] = {25, 146, 190, 62, 174, 250};

int red_hsv[2] = {75, 150};
int green_hsv[2] = {320, 360};
int blue_hsv[2] = {200, 260};

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
	Mat im_hsv;
        im = cv_bridge::toCvShare(msg, "bgr8")->image;
	cvtColor(im, im_hsv, CV_RGB2HSV);
        static bool init_FLAG = init_params(im);

        if(im.empty()){
            ROS_ERROR("failed to convert image");
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
            ROS_ERROR("%s", e.msg);
        }

        for(vector<KeyPoint>::iterator it=keypoints.begin(); it!=keypoints.end(); ++it)
        {
            Scalar color= im.at<Vec3b>(Point(it->pt.x, it->pt.y));
            cout<<"("<<it->pt.x<<","<<it->pt.y<<")"<<"-->"<<im.at<Vec3b>(Point(it->pt.x, it->pt.y))<<endl;
	    if(color[0]>=red_hsv[0]&&color[0]<=red_hsv[1])
                cout<<"Red Blob Detected"<<endl;
            if(color[0]>=green_hsv[0]&&color[0]<=green_hsv[1])
                cout<<"Green Blob Detected"<<endl;
            if(color[0]>=blue_hsv[0]&&color[0]<=blue_hsv[1])
                cout<<"Blue Blob Detected"<<endl;
        }

        imshow("video", im_with_keypoints);
        //must call waitKey so that the window can be updated
        waitKey(30);
    }
    catch(cv_bridge::Exception& e){
        ROS_ERROR("failed to convert image");
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
