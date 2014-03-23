#!/usr/bin/env python2
# license removed for brevity
import rospy
from std_msgs.msg import UInt16

from serial import Serial

def talker():

    s=Serial("/dev/ttyACM0", 9600);
    print("Serial opened")

    pub = rospy.Publisher('chatter', UInt16)
    rospy.init_node('talker')
    while not rospy.is_shutdown():
        print("1000");
        for i in range(1, 1000):
            r=s.read(2);
            k=ord(r[1])*256+ord(r[0])
            pub.publish(k)

if __name__ == '__main__':
    try:
        talker()
    except rospy.ROSInterruptException: pass
