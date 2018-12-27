# -*- coding: utf-8 -*-

import sys
from drone_proxy import *
from std_msgs.msg import String,Float32,Float64
import time, threading

drone=Drone()
#设置初始值，在没有接收到xbee数据时，将执行该目标指令
roll   = 0.0
pitch  = 0.0
yaw    = 0.0
thrust = 20.0
yaw_init = 173.0 #将机体坐标系与地面坐标系对齐

#电机解锁
ret = drone.unlock()
print "进行解锁：",ret.success
time.sleep(2)
while ret.success != True:                   
    print "waiting for unlock"
    ret = drone.unlock()
    time.sleep(2)                              

mode = CtrlMode() #控制模式
mode.hMode = 0
mode.vMode = 0
mode.yawMode = 1
mode.coordinate = 0
ret = drone.flyCtrl.enter(mode)
print "API Control:",ret.success
time.sleep(1)

while ret.success != True:
    ret = drone.flyCtrl.enter(mode)
    time.sleep(1)
    print "waiting for API control"

def safeAngle(Double):
    if Double < -10:
        temp = -10
    elif Double > 10:
        temp = 10
    else:
        temp = Double
    return temp

def change(data):
    temp = data
    return temp
	
def thrustcallback(data):
    global thrust
    thrust = change(data.data)

def rollcallback(data):
    global roll
    roll = safeAngle(data.data)

def pitchcallback(data):
    global pitch
    pitch = -safeAngle(data.data)

def yawcallback(data):
    global yaw
    yaw = change(data.data) + yaw_init

def APIctrl():
    # Subscriber thrust and oriention
    rospy.Subscriber("/cmd/thrust", Float32, thrustcallback)
    rospy.Subscriber("/cmd/roll", Float32, rollcallback)
    rospy.Subscriber("/cmd/pitch", Float32, pitchcallback)
    rospy.Subscriber("/cmd/yaw", Float32, yawcallback)

    rospy.init_node('APIctrl', anonymous=True)
    rate = rospy.Rate(20)

    while not rospy.is_shutdown():
        # set thrust and oriention
        drone.flyCtrl.control(mode, roll, pitch, thrust, yaw)
        print "set_roll = %-7s"%roll, "pitch = %-7s"%pitch, "yaw = %-7s"%yaw, "thrust = %-7s"%thrust
        rate.sleep()
		
if __name__ == '__main__':
    try:
        APIctrl()
    except rospy.ROSInterruptException:
        pass		

