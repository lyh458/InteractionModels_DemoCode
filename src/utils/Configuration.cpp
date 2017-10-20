/*
 * Configuration.cpp
 *
 *  Created on: Mar 15, 2013
 *      Author: ddvogt
 */
#include <iostream>
#include "Configuration.h"
#ifdef WIN32
#include <windows.h>
#endif

namespace iMod {


Configuration& Configuration::getInstance(){
	static Configuration instance; // Guaranteed to be destroyed.
								   // Instantiated on first use.
	return instance;
}

Configuration::Configuration(){

	_read = false;

	char* pPath;
	pPath = getenv("IMOD_ROOT");
	if (pPath != NULL) {
		std::stringstream s;
		s<<pPath<<"CPP/src/configuration.ini";
		this->setPath(s.str());
        UT_NOTIFY(LV_INFO,"Configuration file found.");
	} else
		UT_NOTIFY(LV_FATAL,"Could not load configuration file. Check IMOD_ROOT environment variable");
	
    read();
    createParameterList();
}

Configuration::~Configuration() {
    stopCyclicRead();
	// TODO Auto-generated destructor stub
}

void Configuration::setPath(std::string file){
	boost::mutex::scoped_lock(mx);
	filePath = file;
}

void Configuration::startCyclicRead(){
	boost::mutex::scoped_lock(mx);

	if(_read)
		return;

	_read = true;
    boost::thread readThread(boost::bind(&Configuration::cyclic_read,this));
}

void Configuration::cyclic_read(){
	while(_read){
		read();
#ifdef WIN32
        Sleep(1000);
#else
        sleep(1);
#endif
    }

}
void Configuration::read(){

	boost::mutex::scoped_lock(mx);
	if (filePath.empty()) {
		UT_NOTIFY(LV_FATAL,"You need to set the configurations path first. Aborting ...");
		abort();
	}

	std::filebuf fb;
	if (!fb.open(filePath.c_str(), std::ios::in)) {
		UT_NOTIFY(LV_FATAL,"Couldn't load configuration file. Aborting... ");
		abort();
	}

	std::istream is(&fb);
	// populate tree structure pt
	try{
		boost::property_tree::ini_parser::read_ini(is, _pt);
	}
    catch(boost::property_tree::ini_parser_error& e){
		UT_NOTIFY(LV_ERROR,"Couldn't parse configuration file. Error: "<<e.what());
	}

	//UT_NOTIFY(LV_DEBUG,"Done reading configuration file");
}
    
void Configuration::createParameterList(){
        
		_parameterGroupNameMapping.clear();
        
		// traverse pt
		using boost::property_tree::ptree;
	//	boost::mutex::scoped_lock(mx);
    
    BOOST_FOREACH(const auto &v, _pt.get_child("")) {
            std::vector<std::string> parameters;
            ptree children = _pt.get_child(v.first);
            for(const auto &kv : children){
                parameters.push_back(kv.first);
            }
            _parameterGroupNameMapping.insert(std::pair<std::string,std::vector<std::string> >(v.first,parameters));
        }
    
    
    //_parameterGroupNameMapping.insert(groupName,parameters);
}
#ifdef OS_WIN
Configuration &iMod::getInstanceExtern()
{
    return Configuration::getInstance();
}

void iMod::startCyclicReadExtern()
{
    getInstanceExtern().startCyclicRead();
}
#endif
//UT_NOTIFY(LV_DEBUG,"Done reading configuration file");
    
} /* namespace iMod */
