import sys, argparse
import struct
import time
import RobotRaconteur as RR
import thread
import threading
import random
import numpy


#Create a TcpTransport and register it
t2=RR.TcpTransport()
RR.RobotRaconteurNode.s.RegisterTransport(t2)

#Connect to the service
obj=RR.RobotRaconteurNode.s.ConnectService("tcp://localhost:99999/DataServer/Container" )

while 1:
	print(obj.data)
	time.sleep(1)

#Shutdown !
RRN. Shutdown ( )
