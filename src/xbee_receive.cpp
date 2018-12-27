#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h> 
#include <sstream>
#include "sys/time.h"
#include <math.h>
#include <stdio.h>
#include "serial.h"
#include <std_msgs/Float32.h>

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
	ros::Publisher roll_pub = n.advertise<std_msgs::Float32>("/cmd/roll", 10);
	ros::Publisher pitch_pub = n.advertise<std_msgs::Float32>("/cmd/pitch", 10);
	ros::Publisher yaw_pub = n.advertise<std_msgs::Float32>("/cmd/yaw", 10);
	ros::Publisher thrust_pub = n.advertise<std_msgs::Float32>("/cmd/thrust", 10);

	ros::Rate loop_rate(20);
	short data_out[14];
	int secs[2];
	int fd;

	std_msgs::Float32 roll_receive, pitch_receive, yaw_receive, thrust_receive;
//	double ltime,ntime;
//	ltime=getTime();
//	ntime=ltime;

	fd = serial_open_file("/dev/ttyUSB0", 57600);
	ROS_INFO("Open Serial %d", fd);

	while (ros::ok()) {
//		ntime=getTime();
//		ltime=ntime;

		if(readmessage (fd, data_out)<1)
		{
			loop_rate.sleep();
		}
		memcpy(secs,&data_out[10],sizeof(secs));

		roll_receive.data   = (float)data_out[0]/31416.0f *180;
		pitch_receive.data  = (float)data_out[1]/31416.0f *180;
		yaw_receive.data    = (float)data_out[2]/31416.0f *180;
		thrust_receive.data = (float)data_out[3]/100.0f;

		roll_pub.publish(roll_receive);
		pitch_pub.publish(pitch_receive);
		yaw_pub.publish(yaw_receive);
		thrust_pub.publish(thrust_receive);

//		if(inittime<1){
//			inittime=ros::Time::now().toSec()-secs[0]-secs[1]/1000000000.0f;
//		}
//		double t=(inittime+secs[0]+secs[1]/1000000000.0f);
//		msg.header.stamp.sec=(int)t;
//		msg.header.stamp.nsec=(int)((t-msg.header.stamp.sec)*1000000000);//secs[1];
//		pub.publish(msg);

		std::cout<<roll_receive<<"-"<<pitch_receive<<"-"<<yaw_receive<<"----"<<thrust_receive<<std::endl;

		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
