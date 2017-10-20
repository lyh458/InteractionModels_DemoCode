/*
 * unitTest.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include <memory>
#include <chrono>

#include "Baxter.h"
#include "../../utils/notify.h"
#include "../../utils/Configuration.h"

int main(){

    UT_NOTIFY(LV_INFO,"Creating Baxter");

    std::string ip = iMod::Configuration::getInstance().getParameter<std::string>("Baxter","IP");
    int jointPort = iMod::Configuration::getInstance().getParameter<int>("Baxter","JointServicePort");
    int peripheralsPort = iMod::Configuration::getInstance().getParameter<int>("Baxter","PeripheralsServicePort");
    std::shared_ptr<iModBaxter::Baxter> baxter(new iModBaxter::Baxter(ip,jointPort,peripheralsPort));
    baxter->init();

    std::vector<double> d;
    std::vector<double> jointValues;
    baxter->getJointAngles(d);

    for(const auto it : d)
        jointValues.push_back(0.5);

    std::chrono::time_point<std::chrono::system_clock> start, end;
    start = std::chrono::system_clock::now();
    int frames = 1000;
    int counter = 0;
    while(1){
        baxter->getJointAngles(d);
        d.clear();

        if(counter == frames) break;
        else ++counter;
    }

    end = std::chrono::system_clock::now();
    std::chrono::duration<double> elapsed_seconds = end-start;

    UT_NOTIFY(LV_INFO,"Receiving joint angles finished in "<< elapsed_seconds.count() << "s. FPS=" << frames/elapsed_seconds.count());

    UT_NOTIFY(LV_INFO,"Setting Baxter control mode");
    baxter->setControlMode(iModBaxter::ControlMode::torqueControl);

    UT_NOTIFY(LV_INFO,"Setting Baxters joint angles");
    baxter->setJointAngles(jointValues,jointValues);

    UT_NOTIFY(LV_INFO,"Paning Baxters head");
    baxter->panHead(3.14/4);


    baxter->close();
	}
