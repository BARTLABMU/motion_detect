#include <ros/ros.h>
#include <image_transport/image_transport.h>            //send image between node
#include <opencv/cv.h>                                  // opencv
#include <opencv/highgui.h>
#include <cv_bridge/cv_bridge.h>
#include <sensor_msgs/image_encodings.h>                // open camera
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <zbar.h>
#include <string.h>
# include <unistd.h>
#include "std_msgs/String.h"
#include <sstream>
#include <fstream>
#include <iostream>

using namespace cv;                                     //
using namespace std;
using namespace zbar;
namespace enc = sensor_msgs::image_encodings;
ros::Publisher pub;
ros::Publisher qr_pub;
std::string data = "data";
bool is_capture = false;
Mat im;
int direction=0;
int i = 1, id =1;
cv_bridge::CvImageConstPtr cv_ptr;                      // change sensor img msg to CV struture
  char filename[40];
//static int num_images = 1, num_symbols = 0;
void imageCallback(const sensor_msgs::ImageConstPtr& msg)
{

    //imsignal.data = "Y";

   Mat image;                                           // matrix(CV Structure) of image
   Mat subim;                                           // subtract image
   std_msgs::String imsignal;                           //
     cv_ptr =  cv_bridge::toCvShare(msg, enc::MONO8);   // encode image into Mono scale 8 bits
  int m;
      if(is_capture==false)                             //
      {
          if(i == 1)
          {


           im = cv_ptr->image;



   //imshow("im",im);
     //   cvWaitKey(3);
           i++;
         }
          else if(i ==2)
          {
              cv_ptr =  cv_bridge::toCvShare(msg, enc::MONO8);
               image = cv_ptr->image;

              subim = im-image;

              threshold(subim,subim,50,255,CV_THRESH_BINARY);

                imshow("subim",subim);
               waitKey(1);
                m=countNonZero(subim);
            //     ROS_INFO("%d",m);
                if(m > 1500)
                {   ROS_INFO("MOTION DETECTED");
                    imsignal.data = "MOTION DETECTED";
                    pub.publish(imsignal);

                }
                else
                {ROS_INFO("NO MOTION");
                    imsignal.data = "NO MOTION";
                    pub.publish(imsignal);
                }

            //  is_capture = false;
               i = 1;
          }
}







}
void com_callback(const std_msgs::StringConstPtr &msg)
{
    if(msg->data == "motion"&&is_capture == false)
   {
        is_capture = true;

    }
    else if(msg->data == "stop"&&is_capture == true)
    {
        is_capture = false;
    }

}

int main(int argc, char **argv)
{

  ros::init(argc, argv, "image_listener");
  ros::NodeHandle nh;
  ros::NodeHandle n;
  pub = n.advertise<std_msgs::String>("motion_pub", 1);
  qr_pub = n.advertise<std_msgs::String>("qrdata", 1);
// cv::namedWindow("QR");
     ros::Subscriber com_sub = n.subscribe("auto_com",1,com_callback);

     namedWindow("im");
     namedWindow("subim");
  image_transport::ImageTransport it(nh);
  image_transport::Subscriber sub = it.subscribe("/usb_cam_1/image_raw", 1, imageCallback);
  ros::spin();




}
