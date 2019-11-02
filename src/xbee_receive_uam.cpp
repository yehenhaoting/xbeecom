#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h> 
#include <sstream>
#include "sys/time.h"
#include <stdio.h>
#include "serial.h"

double inittime=0;

double getTime()
{
    struct timeval t;
    gettimeofday(&t,NULL);
    double rt=t.tv_usec;
    rt=rt/1000000+t.tv_sec;
    return rt;
}


int main(int argc, char **argv) {
	ros::init(argc, argv, "xbee_receiver");
	ros::NodeHandle n;
	ros::Publisher uam_base_pub = n.advertise<geometry_msgs::PoseStamped>("/uam/base_pose", 10);

	ros::Rate loop_rate(20);
	short data_out[14];
	int secs[2];
	int fd;

	geometry_msgs::PoseStamped base_pose;
//	double ltime,ntime;
//	ltime=getTime();
//	ntime=ltime;

	fd = serial_open_file((char*)"/dev/ttyUSB1", 19200);
	ROS_INFO("Open Serial %d", fd);

	while (ros::ok()) {
//		ntime=getTime();
//		ltime=ntime;

		if(readmessage (fd, data_out)<1)
		{
			loop_rate.sleep();
		}
		memcpy(secs,&data_out[10],sizeof(secs));

		base_pose.pose.position.x = (float)data_out[0]/1000;
		base_pose.pose.position.y = (float)data_out[1]/1000;
		base_pose.pose.position.z = (float)data_out[2]/1000;
		base_pose.pose.orientation.x = (float)data_out[3]/1000;
		base_pose.pose.orientation.y = (float)data_out[4]/1000;
		base_pose.pose.orientation.z = (float)data_out[5]/1000;
		base_pose.pose.orientation.w = (float)data_out[6]/1000;

        ROS_INFO_STREAM(base_pose.pose);
        uam_base_pub.publish(base_pose);

//		if(inittime<1){
//			inittime=ros::Time::now().toSec()-secs[0]-secs[1]/1000000000.0f;
//		}
//		double t=(inittime+secs[0]+secs[1]/1000000000.0f);
//		msg.header.stamp.sec=(int)t;
//		msg.header.stamp.nsec=(int)((t-msg.header.stamp.sec)*1000000000);//secs[1];
//		pub.publish(msg);

		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
