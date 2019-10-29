#include "ros/ros.h"
#include "std_msgs/String.h"
#include <mavros_msgs/AttitudeTarget.h>
#include <sstream>
#include "sys/time.h"
#include <math.h>
#include <stdio.h>
#include "serial.h"

int initsecs;
int fd;

void attitudeHandler(mavros_msgs::AttitudeTarget msg)
{
    short data[14];
    int secs[2];
    data[0]=(short)(msg.body_rate.x*10000);
    data[1]=(short)(msg.body_rate.y*10000);
    data[2]=(short)(msg.body_rate.z*10000);
    data[3]=(short)(msg.thrust*10000);
    data[4]=(short)(-1*10000);
    data[5]=(short)(-1*10000);
    data[6]=(short)(-1*10000);
    data[7]=(short)(-1*10000);
    data[8]=(short)(-1*10000);
    data[9]=(short)(-1*10000);
    secs[0]=msg.header.stamp.sec;
    secs[1]=msg.header.stamp.nsec;
    std::cout<<secs[0]<<":"<<secs[1]<<":"<<msg.body_rate.x<<":"<<msg.body_rate.y<<":"<<msg.body_rate.z<<":"<<msg.thrust<<std::endl;
    memcpy(&data[10],secs,sizeof(secs));

    if(fd<1)
    {
        fd = serial_open_file((char*)"/dev/ttyUSB0", 57600);
    }
    sendmessage(fd,data);
}

int main(int argc, char **argv) {
	ros::init(argc, argv, "xbee_sender");
	ros::NodeHandle n;
    fd = serial_open_file((char*)"/dev/ttyUSB0", 57600);
    ROS_INFO("Open Serial: [%d]", fd);

	initsecs = (int)(ros::Time::now().toSec());
	ros::Subscriber sub = n.subscribe("/cmd/attitudeTarget", 10, attitudeHandler);
	ros::spin();
	return 0;
}
