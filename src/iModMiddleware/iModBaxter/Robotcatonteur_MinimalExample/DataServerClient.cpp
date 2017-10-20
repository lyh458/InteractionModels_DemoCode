/*
 * unitTest.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include <memory>
#include <chrono>

#include <iostream>
#include <sstream>      // std::stringstream
#include <string>
#include <vector>

#include "../../../utils/Configuration.h"
#include "../../../utils/notify.h"

#include "service__data.h"
#include "service__data_stubskel.h"

int main(){

    UT_NOTIFY(LV_INFO,"C++ Robotraconteur Demo");
    std::string uri ="tcp://localhost:99999/DataServer/Container";
    boost::shared_ptr<RobotRaconteur::TcpTransport> tcp=boost::make_shared<RobotRaconteur::TcpTransport>();
    RobotRaconteur::RobotRaconteurNode::s()->RegisterTransport(tcp);
    RobotRaconteur::RobotRaconteurNode::s()->RegisterServiceType(boost::make_shared<service::data::service__dataFactory>());
    boost::shared_ptr<service::data::Container> dataService = RobotRaconteur::rr_cast<service::data::Container>(RobotRaconteur::RobotRaconteurNode::s()->ConnectService(uri,"",boost::shared_ptr<RobotRaconteur::RRMap<std::string,RobotRaconteur::RRObject> >(),NULL,"service.data.Container"));


    while(1){

        RR_SHARED_PTR<RobotRaconteur::RRArray<double > > data = dataService->get_data();
        for(auto i=0;i<data->size();i++)
            std::cout<<data->operator [](i)<<" ";
        std::cout<<std::endl;
        sleep(1);
    }

	}
