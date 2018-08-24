import socket
import time
import datetime
import os
import sys

ip_addr = "192.168.4.1"
port = 5006

fusion_path = "/dev/FUSION"

sock_tcp = socket.socket(socket.AF_INET, socket.SOCK_STREAM)
sock_tcp.bind((ip_addr, port))
sock_tcp.listen(1)
client, addr = sock_tcp.accept()

counter = 0

class Packet():
    def __init__(self, data_string):
        self.raw_data = data_string
        self.deserialize()
    def getData(self):
        return self.data
    def getRawData(self):
        return self.raw_data
    def deserialize(self):
        self.raw_bytes = list(self.raw_data[0])
        self.ni = self.raw_bytes[3]
        self.heartbeat = self.raw_bytes[1]
        self.data_length = self.raw_bytes[4]
        self.data = self.raw_bytes[5:5 + self.data_length]
        self.checksum = self.raw_bytes[5 + self.data_length:7+self.data_length]
        self.checksum_ok = True

while True:
    print(counter)
    client.send(str(counter))
    counter += 1
#   data = client.recvfrom(1024)
#   pack = Packet(data)
#   try:
#       os.stat(fusion_path)
#   except:
#       os.makedirs(fusion_path)

#   fifo_path = fusion_path + "/node{}_in".format(pack.ni)

#   outfile = open(fifo_path, "w")
#   outfile.write(str(pack.ni) + "\n")
#   outfile.write(str(pack.heartbeat) + "\n")

#   for d in pack.getData():
#       outfile.write(str(d))
#   outfile.write("\n")
#   
#   outfile.write(str(time.time()) + "\n")
#   outfile.close()
    
    time.sleep(1)
