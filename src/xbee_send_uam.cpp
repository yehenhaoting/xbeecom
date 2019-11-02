#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h>
#include <sstream>
#include <stdio.h>
#include "serial.h"

int initsecs;
int fd;

void pose_callback(geometry_msgs::PoseStamped msg)
{
    short data[14];
    int secs[2];
    data[0]=(short)(msg.pose.position.x*1000);
    data[1]=(short)(msg.pose.position.y*1000);
    data[2]=(short)(msg.pose.position.z*1000);
    data[3]=(short)(msg.pose.orientation.x*1000);
    data[4]=(short)(msg.pose.orientation.y*1000);
    data[5]=(short)(msg.pose.orientation.z*1000);
    data[6]=(short)(msg.pose.orientation.w*1000);
    data[7]=(short)(-1*1000);
    data[8]=(short)(-1*1000);
    data[9]=(short)(-1*1000);
//    secs[0]=msg.header.stamp.sec;
//    secs[1]=msg.header.stamp.nsec;
    secs[0]=-1;
    secs[1]=-1;
    std::cout<<secs[0]<<"-"<<secs[1]<<"\tx:"<<msg.pose.position.x<<"\ty:"<<msg.pose.position.y<<"\tz:"<<msg.pose.position.z<<std::endl;
    memcpy(&data[10],secs,sizeof(secs));

    if(fd<1)
    {
        fd = serial_open_file((char*)"/dev/ttyUSB0", 19200);    //upboard 的波特率为19200
    }
    sendmessage(fd,data);
}

int main(int argc, char **argv) {
	ros::init(argc, argv, "xbee_sender_uam");
	ros::NodeHandle n;
    fd = serial_open_file((char*)"/dev/ttyUSB0", 19200);
    ROS_INFO("Open Serial: [%d]", fd);

	initsecs = (int)(ros::Time::now().toSec());
	ros::Subscriber sub = n.subscribe("/mocap/pose", 10, pose_callback);
	ros::spin();
	return 0;
}
