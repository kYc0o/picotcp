#!/usr/bin/python
from  topology import *
import socket, random, string

SRC_IP_UDP = ''
DST_IP_UDP = '172.16.1.1'
SRC_PORT_UDP = 6667
DST_PORT_UDP = 6667
DATASIZE_UDP = 4000
LOOPS_UDP = 4
SUBLOOPS_UDP = 1
UDPCLIENT = "udpclient:" + str(DST_IP_UDP) + ":" + str(DST_PORT_UDP) + ":" + str(DATASIZE_UDP) + ":" + str(LOOPS_UDP) + ":" + str(SUBLOOPS_UDP)

print UDPCLIENT

T = Topology()
net1 = Network(T, "pyt0")
h1 = Host(T, net1, args=UDPCLIENT)

raw_input("Created device pyt0, press enter ...")
s_udp = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
s_udp.bind((SRC_IP_UDP, SRC_PORT_UDP))
start(T)

while True:
  data, addr = s_udp.recvfrom(DATASIZE_UDP) 
  print "received message: ", data

wait(h1)
cleanup()
