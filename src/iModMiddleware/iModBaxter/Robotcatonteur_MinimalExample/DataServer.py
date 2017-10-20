import sys, argparse
import struct
import time
import RobotRaconteur as RR
import thread
import threading
import random
import numpy

baxter_servicedef="""
#Service to provide sample interface to the iRobot Create
service service.data

option version 0.5

object Container
    property double[] data
end object
"""

class Container_impl(object):
    def __init__(self):
        print "Initializing Node"

        self._stopped = False
        self._data = [1,2,3,4,5]

    @property   
    def data(self):
        return self._data

    def start(self):
        self._runProcess = threading.Thread(target=self.run)
        self._runProcess.start()

    def close(self):
        self._stopped = True
        self._runProcess.stop()

    def run(self):
        while not self._stopped:
            self._data = [random.randint(1, 10),random.randint(1, 10),random.randint(1, 10),random.randint(1, 10),random.randint(1, 10)]
            time.sleep(1)

def main(argv):
    # parse command line arguments
    parser = argparse.ArgumentParser(description='Initialize Data Server.')
    parser.add_argument('--port', type=int, default = 0,
                    help='TCP port to host service on' + \
                           '(will auto-generate if not specified)')
    args = parser.parse_args(argv)

    #Enable numpy
    RR.RobotRaconteurNode.s.UseNumPy=True

    #Set the Node name
    RR.RobotRaconteurNode.s.NodeName="DataServer"

    #Initialize object
    container_obj = Container_impl()
    container_obj.start()

    #Create transport, register it, and start the server
    print "Registering Transport"
    t = RR.TcpTransport()
    t.EnableNodeAnnounce(RR.IPNodeDiscoveryFlags_NODE_LOCAL | 
                         RR.IPNodeDiscoveryFlags_LINK_LOCAL | 
                         RR.IPNodeDiscoveryFlags_SITE_LOCAL)

    RR.RobotRaconteurNode.s.RegisterTransport(t)
    t.StartServer(args.port)
    port = args.port
    if (port == 0):
        port = t.GetListenPort()

    #Register the service type and the service
    print "Starting Service"
    RR.RobotRaconteurNode.s.RegisterServiceType(baxter_servicedef)
    RR.RobotRaconteurNode.s.RegisterService("Container",
                      "service.data.Container",container_obj)

    print "Service started, connect via"
    print "tcp://localhost:" + str(port) + "/DataServer/Container"
    raw_input("press enter to quit...\r\n")
    
    container_obj.close()
    
    # This must be here to prevent segfault
    RR.RobotRaconteurNode.s.Shutdown()

if __name__ == '__main__':
    main(sys.argv[1:])
