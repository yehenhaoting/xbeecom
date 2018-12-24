#include "ros/ros.h"
#include "std_msgs/String.h"
#include <geometry_msgs/PoseStamped.h> 
#include <sstream>
#include "sys/time.h"
#include <math.h>
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
	ros::Publisher pub = n.advertise<geometry_msgs::PoseStamped>("/mavros/mocap/pose", 10);
	ros::Publisher pubref = n.advertise<geometry_msgs::PoseStamped>("/mavros/setpoint_position/local", 10);
	ros::Rate loop_rate(30);
	short data_out[14];
	int secs[2];
	int fd;
	int data_new_flag=0;
	geometry_msgs::PoseStamped msg;
	geometry_msgs::PoseStamped msgref;
	double ltime,ntime;
	ltime=getTime();
	ntime=ltime;
	/*
	msg.layout.dim[0].size=2;
	msg.layout.dim[0].stride=2;
	msg.layout.dim[0].label="Time";
	msg.layout.dim[1].size=1;
	msg.layout.dim[2].size=1;
	*/
	fd = serial_open_file("/dev/ttyUSB0", 57600);
	ROS_INFO("Open Serial %d", fd);
	//openFtdi = open_ftdi(57600, "Aero0", 5, 15);
	//ROS_INFO("Open Ftdi: [%d]", openFtdi);
	while (ros::ok()) {
		ntime=getTime();
		ltime=ntime;

		if(readmessage (fd, data_out)<1)
		{
			loop_rate.sleep();
		}
		memcpy(secs,&data_out[10],sizeof(secs));
		msg.pose.position.x=data_out[0]/1000.0f;
		msg.pose.position.y=data_out[1]/1000.0f;
		msg.pose.position.z=data_out[2]/1000.0f;
		msg.pose.orientation.x=data_out[3]/1000.0f;
		msg.pose.orientation.y=data_out[4]/1000.0f;
		msg.pose.orientation.z=data_out[5]/1000.0f;
		msg.pose.orientation.w=data_out[6]/1000.0f;
		if(inittime<1){
			inittime=ros::Time::now().toSec()-secs[0]-secs[1]/1000000000.0f;
		}
		double t=(inittime+secs[0]+secs[1]/1000000000.0f);
		msg.header.stamp.sec=(int)t;
		msg.header.stamp.nsec=(int)((t-msg.header.stamp.sec)*1000000000);//secs[1];
		pub.publish(msg);
		msgref.pose.position.x=data_out[7]/1000.0f;
		msgref.pose.position.y=data_out[8]/1000.0f;
		msgref.pose.position.z=data_out[9]/1000.0f;
		std::cout<<data_out[0]<<"-"<<data_out[8]<<std::endl;
		if(msgref.pose.position.z<-5)
		{
			msgref.pose.position.x=msg.pose.position.x;
			msgref.pose.position.y=msg.pose.position.y;
			msgref.pose.position.z=msg.pose.position.z-5;
		}
		if(msgref.pose.position.z>2)
		{
			msgref.pose.position.z=2;
		}
		pubref.publish(msgref);

		ros::spinOnce();
		loop_rate.sleep();
	}
	return 0;
}
