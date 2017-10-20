/*
 * ART.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include "ART.h"
#include <cstdlib>

#ifdef WIN32
#include <qmath.h>
#include <QtCore/QDebug>
#endif

namespace iModTracker {

ART::ART() {
    // TODO Auto-generated constructor stub
#ifdef WIN32
    qInfo() << "Starting ART-Library";
    _convertToMeter = iMod::getInstanceExtern().getParameter<int>("ART","convertARTDataToMeter");
    _convertToDeree = iMod::getInstanceExtern().getParameter<int>("ART","convertARTDataToDegree");
    _refreshRate = iMod::getInstanceExtern().getParameter<int>("ART","refreshRate");

    iMod::startCyclicReadExtern();
#else
    _convertToMeter = iMod::Configuration::getInstance().getParameter<int>("ART","convertARTDataToMeter");
    _convertToDeree = iMod::Configuration::getInstance().getParameter<int>("ART","convertARTDataToDegree");
    _refreshRate = iMod::Configuration::getInstance().getParameter<int>("ART","refreshRate");
    
    iMod::Configuration::getInstance().startCyclicRead();
#endif

    //Initalizing variables
    _updateTargets = true;
    _validSQL = false;
    dt = nullptr;
}

ART::~ART() {
    //Join the reading thread
    stop();
    delete dt;
}


TargetPtr ART::getTarget(const int &id){

    TargetMapT::iterator i;
    boost::unique_lock<boost::mutex> lock(mx);
    
    i = targets.find(id);
    if(i!= targets.end()){
        //TargetPtr t = i->second;
        //std::cout<<t->getPosition()[0]<<" "<<t->getPosition()[1]<<" "<<t->getPosition()[2]<<std::endl;
        return i->second;
    }
    else
        return TargetPtr();//unlocked here
}

void ART::registerTargets(){
    std::vector<double> trackerIDs;

#ifdef WIN32
    auto targetNames = iMod::getInstanceExtern().getParameterList("ART_Targets");
#else
    auto targetNames = iMod::Configuration::getInstance().getParameterList("ART_Targets");
#endif
    targets.clear();

    double id = 0;
    std::cout<<"Creating ART targets from configuration file: ";
    for(auto &i : targetNames){
        TargetPtr t(new Target());
        t->setName(i);
        //targets[trackerIDs[i]] = t;
#ifdef WIN32
        id = iMod::getInstanceExtern().getParameter<double>("ART_Targets",i);
#else
        id = iMod::Configuration::getInstance().getParameter<double>("ART_Targets",i);
#endif
        targets.insert(std::pair<double, TargetPtr>(id,t));
        std::cout<<" "<<i<<" id="<<id;
    }
    std::cout<<std::endl;
}


void ART::start(){
    if(dt == nullptr)
        dt = new DTrackSDK(
            #ifdef WIN32
                    iMod::getInstanceExtern().getParameter<std::string>("ART","ServerAdress"),
                    iMod::getInstanceExtern().getParameter<int>("ART","ServerPort")
            #else
                    iMod::Configuration::getInstance().getParameter<std::string>("ART","ServerAdress"),
                    iMod::Configuration::getInstance().getParameter<int>("ART","ServerPort")
            #endif
                    );

    if(!dt->isLocalDataPortValid() || !dt->isCommandInterfaceValid()){
        std::cout << "DTrackSDK init error" << std::endl;
        messages_to_console();
        return;
    }

    // start measurement:
    if (!dt->startMeasurement()) {
        std::cout << "Start measurement failed! Trying again..." << std::endl;

        //Try to stop it and then try again
        dt->stopMeasurement();
        dt->startMeasurement();

        messages_to_console();
    }

    _updateTargets = true;

    readTargetsFromServerThread = new boost::thread(boost::bind(&ART::update,this));
}

void ART::stop(){
    _updateTargets = false;

    readTargetsFromServerThread->join();
    delete readTargetsFromServerThread;

    if(dt!=NULL)
        dt->stopMeasurement();

}//unlocked here


void ART::update(){
    DTrack_Body_Type_d body;
    TargetMapT::iterator iter;

    unsigned int tmpFrameCounter = 0;
    unsigned int _frameCounter = 0;

    while(_updateTargets){
        iMod::Timer_T<boost::chrono::high_resolution_clock> clock;
#ifdef WIN32
        _refreshRate = iMod::getInstanceExtern().getParameter<int>("ART","refreshRate");
#else
        _refreshRate = iMod::Configuration::getInstance().getParameter<int>("ART","refreshRate");
#endif

        if(!dt->receive()){
            //nothing received, wait a while and try again
            std::cout<<"No data received from A.R.T server"<<std::endl;
            boost::this_thread::sleep(boost::posix_time::milliseconds(125));
            continue;
        }

        bool firstTarget = true;
        std::stringstream sql;
        if(_validSQL)
            sql<<createSQlTableString();

        //printf("Bodys: %d\n", dt->getNumBody());
        for(int i=0; i<dt->getNumBody(); i++){
            body = *dt->getBody(i);

            //Marker registered but not tracked
            //printf("Quality: %.2lf\n", body.quality);
            if(body.quality < 0)
                continue;

            //Check if we want to listen to that target
            iter = targets.find(body.id);
            //printf("Body-ID: %d\n", body.id);

            //Marker tracked but not registered
            if(iter == targets.end()){
                std::cout<<"Skipping marker with id "<<body.id<<" since it is not recognized"<<std::endl;
                continue;
            }

            if(tmpFrameCounter < dt->getFrameCounter()){
                tmpFrameCounter = dt->getFrameCounter();
                _frameCounter++;
            }

            if(_validSQL)
            {
                if(!firstTarget)
                    sql << ",";
                else
                    firstTarget = false;
                //std::cout << "X: " << body.loc[0] << std::endl;
                sql<<createSQlTargetString(body, _frameCounter);
            }

            if(_convertToMeter){
                for(int j=0;j<3;++j)
                    body.loc[j] = body.loc[j]/1000;
            }

            if(_convertToDeree){
                for(int j=0;j<9;++j)
                    body.rot[j] = body.rot[j]*180/M_PI;
            }

            {
                boost::lock_guard<boost::mutex> lock(mx);
                iter->second->setPosition(body.loc);
                iter->second->setRotation(body.rot);
            } //unlocked here
        }

        //messages_to_console();
        double sleepTime = 1000/_refreshRate-clock.seconds()*1000;
        //std::cout<<"Sleeping (ms) "<<sleepTime<<" refreshRate" <<_refreshRate<<" Elapsed "<<clock.seconds()<<std::endl;
        boost::this_thread::sleep(boost::posix_time::milliseconds(sleepTime));
        //system("clear");
    }
}

void ART::messages_to_console(){
    while(dt->getMessage()){
        std::cout << "ATC message: \"" << dt->getMessageStatus() << "\" \"" << dt->getMessageMsg() << "\"" << std::endl;
    }
}

std::string ART::createSQlTableString(){
    std::stringstream str;
    str<< "INSERT INTO " << tableName << " (id, quality, framecounter, loc_1, loc_2, loc_3, rot_1, rot_2, rot_3, rot_4, rot_5, rot_6, rot_7, rot_8, rot_9) VALUES ";
    return str.str();
}

std::string ART::createSQlTargetString(DTrack_Body_Type_d &body, unsigned int &_frameCounter )
{
    std::stringstream sql;
    sql << "(" << body.id <<","<< body.quality <<","
        << _frameCounter << ","
        << body.loc[0] <<","<< body.loc[1] <<","<< body.loc[2] <<","
        << body.rot[0] <<","<< body.rot[1] <<","<< body.rot[2] <<","<< body.rot[3] <<","<< body.rot[4] << ","<< body.rot[5] <<","<< body.rot[6] <<","<< body.rot[7] <<","<< body.rot[8] << ")";
    return sql.str();
}

/**
 * 	\brief Prints current tracking data to console.
 */
void ART::output_to_console(){
    system("clear");

    using namespace std;
    cout.precision(3);
    cout.setf(ios::fixed, ios::floatfield);

    cout << endl << "frame " << dt->getFrameCounter() << " ts " << dt->getTimeStamp()
         << " nbod " << dt->getNumBody() << " nfly " << dt->getNumFlyStick()
         << " nmea " << dt->getNumMeaTool() << " nmearef " << dt->getNumMeaRef()
         << " nhand " << dt->getNumHand() << " nmar " << dt->getNumMarker()
         << " nhuman " << dt->getNumHuman()
         << endl;

    // bodies:
    DTrack_Body_Type_d body;
    for(int i=0; i<dt->getNumBody(); i++){
        body = *dt->getBody(i);

        if(body.quality < 0){
            cout << "bod " << body.id << " not tracked" << endl;
        }else{
            cout << "bod " << body.id << " qu " << body.quality
                 << " loc " << body.loc[0] << " " << body.loc[1] << " " << body.loc[2]
                 << " rot " << body.rot[0] << " " << body.rot[1] << " " << body.rot[2]
                 << " " << body.rot[3] << " " << body.rot[4] << " " << body.rot[5]
                 << " " << body.rot[6] << " " << body.rot[7] << " " << body.rot[8] << endl;
        }
    }

    // A.R.T. Flysticks:
    DTrack_FlyStick_Type_d flystick;
    for(int i=0; i<dt->getNumFlyStick(); i++){
        flystick = *dt->getFlyStick(i);

        if(flystick.quality < 0){
            cout << "fly " << flystick.id << " not tracked" << endl;
        }else{
            cout << "flystick " << flystick.id << " qu " << flystick.quality
                 << " loc " << flystick.loc[0] << " " << flystick.loc[1] << " " << flystick.loc[2]
                 << " rot " << flystick.rot[0] << " " << flystick.rot[1] << " " << flystick.rot[2]
                 << " " << flystick.rot[3] << " " << flystick.rot[4] << " " << flystick.rot[5]
                 << " " << flystick.rot[6] << " " << flystick.rot[7] << " " << flystick.rot[8] << endl;
        }

        cout << "      btn";
        for(int j=0; j<flystick.num_button; j++){
            cout << " " << flystick.button[j];
        }

        cout << " joy";
        for(int j=0; j<flystick.num_joystick; j++){
            cout << " " << flystick.joystick[j];
        }

        cout << endl;
    }

    // measurement tools:
    DTrack_MeaTool_Type_d meatool;
    for(int i=0; i<dt->getNumMeaTool(); i++){
        meatool = *dt->getMeaTool(i);

        if(meatool.quality < 0){
            cout << "mea " << meatool.id << " not tracked" << endl;
        }else{
            cout << "mea " << meatool.id << " qu " << meatool.quality
                 << " loc " << meatool.loc[0] << " " << meatool.loc[1] << " " << meatool.loc[2]
                 << " rot " << meatool.rot[0] << " " << meatool.rot[1] << " " << meatool.rot[2]
                 << " " << meatool.rot[3] << " " << meatool.rot[4] << " " << meatool.rot[5]
                 << " " << meatool.rot[6] << " " << meatool.rot[7] << " " << meatool.rot[8] << endl;
        }

        if (meatool.tipradius) {
            cout << "      radius " << meatool.tipradius << endl;
        }

        if (meatool.num_button) {
            cout << "      btn";
            for(int j=0; j<meatool.num_button; j++){
                cout << " " << meatool.button[j];
            }
            cout << endl;
        }
    }

    // measurement references:
    DTrack_MeaRef_Type_d mearef;
    for(int i=0; i<dt->getNumMeaRef(); i++){
        mearef = *dt->getMeaRef(i);

        if(mearef.quality < 0){
            cout << "mearef " << mearef.id << " not tracked" << endl;
        }else{
            cout << "mearef " << mearef.id << " qu " << mearef.quality
                 << " loc " << mearef.loc[0] << " " << mearef.loc[1] << " " << mearef.loc[2]
                 << " rot " << mearef.rot[0] << " " << mearef.rot[1] << " " << mearef.rot[2]
                 << " " << mearef.rot[3] << " " << mearef.rot[4] << " " << mearef.rot[5]
                 << " " << mearef.rot[6] << " " << mearef.rot[7] << " " << mearef.rot[8] << endl;
        }
    }

    // markers:
    DTrack_Marker_Type_d marker;
    for(int i=0; i<dt->getNumMarker(); i++){
        marker = *dt->getMarker(i);

        cout << "mar " << marker.id << " qu " << marker.quality
             << " loc " << marker.loc[0] << " " << marker.loc[1] << " " << marker.loc[2] << endl;
    }

    // A.R.T. Fingertracking hands:
    DTrack_Hand_Type_d hand;
    for(int i=0; i<dt->getNumHand(); i++){
        hand = *dt->getHand(i);

        if(hand.quality < 0){
            cout << "hand " << hand.id << " not tracked" << endl;
        }else{
            cout << "hand " << hand.id << " qu " << hand.quality
                 << " lr " << ((hand.lr == 0) ? "left" : "right") << " nf " << hand.nfinger
                 << " loc " << hand.loc[0] << " " << hand.loc[1] << " " << hand.loc[2]
                 << " rot " << hand.rot[0] << " " << hand.rot[1] << " " << hand.rot[2]
                 << " " << hand.rot[3] << " " << hand.rot[4] << " " << hand.rot[5]
                 << " " << hand.rot[6] << " " << hand.rot[7] << " " << hand.rot[8] << endl;

            for(int j=0; j<hand.nfinger; j++){
                cout << "       fi " << j
                     << " loc " << hand.finger[j].loc[0] << " " << hand.finger[j].loc[1] << " " << hand.finger[j].loc[2]
                     << " rot " << hand.finger[j].rot[0] << " " << hand.finger[j].rot[1] << " " << hand.finger[j].rot[2]
                     << " " << hand.finger[j].rot[3] << " " << hand.finger[j].rot[4] << " " << hand.finger[j].rot[5]
                     << " " << hand.finger[j].rot[6] << " " << hand.finger[j].rot[7] << " " << hand.finger[j].rot[8] << endl;
                cout << "       fi " << j
                     << " tip " << hand.finger[j].radiustip
                     << " pha " << hand.finger[j].lengthphalanx[0] << " " << hand.finger[j].lengthphalanx[1]
                     << " " << hand.finger[j].lengthphalanx[2]
                     << " ang " << hand.finger[j].anglephalanx[0] << " " << hand.finger[j].anglephalanx[1] << endl;
            }
        }
    }

    // A.R.T human model (6dj)
    DTrack_Human_Type human;

    if (dt->getNumHuman() < 1) {
        cout << "no human model data" << endl;
    }

    for(int i=0; i<dt->getNumHuman(); i++){
        human = *dt->getHuman(i);
        cout << "human " << human.id << " num joints " << human.num_joints << endl;
        for (int j=0; j<human.num_joints; j++){
            if(human.joint[j].quality<0){
                cout << "joint " << human.joint[j].id << " not tracked" << endl;
            }else{
                cout << "joint " << human.joint[j].id << " qu " << human.joint[j].quality
                     << " loc "  << human.joint[j].loc[0] << " " << human.joint[j].loc[1] << " " << human.joint[j].loc[2]
                     << " ang "  << human.joint[j].ang[0] << " " << human.joint[j].ang[1] << " " << human.joint[j].ang[2]
                     << " rot "  << human.joint[j].rot[0] << " " << human.joint[j].rot[1] << " " << human.joint[j].rot[2]
                     << " "      << human.joint[j].rot[3] << " " << human.joint[j].rot[4] << " " << human.joint[j].rot[5]
                     << " "      << human.joint[j].rot[6] << " " << human.joint[j].rot[7] << " " << human.joint[j].rot[8]
                     << endl;
            }
        }
        cout << endl;
    }

    // A.R.T inertial bodies(6di)
    DTrack_Inertial_Type_d inertial;

    if (dt->getNumInertial() < 1) {
        cout << "no inertial body data" << endl;
    }

    for(int i=0; i<dt->getNumInertial(); i++){
        inertial = *dt->getInertial(i);
        cout << " inertial body " << inertial.id << " st " << inertial.st << " error " << inertial.error << endl;
        if(inertial.st > 0){
            cout << " loc " << inertial.loc[0] << " " << inertial.loc[1] << " " << inertial.loc[2]
                 << " rot " << inertial.rot[0] << " " << inertial.rot[1] << " " << inertial.rot[2]
                 << " " << inertial.rot[3] << " " << inertial.rot[4] << " " << inertial.rot[5]
                 << " " << inertial.rot[6] << " " << inertial.rot[7] << " " << inertial.rot[8] << endl;
        }
    }
}

} /* namespace iModTracker */

