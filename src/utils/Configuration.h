/*
 * Configuration.h
 *
 *  Created on: Mar 15, 2013
 *      Author: ddvogt
 */

#ifndef CONFIGURATION_H_
#define CONFIGURATION_H_

#include <string>
#include <boost/foreach.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include <boost/property_tree/ptree.hpp>
#include <boost/thread.hpp>

#ifdef WIN32
#include "notify_windows.h"
#else
#include "notify.h"
#endif

namespace iMod {


/*!
 * This class loads a specific configuration.
 * The environment variable IMOD_ROOT has to be set.
 * */
class Configuration {
    /*!
         * The tree storing the read XML structure
         */
    boost::property_tree::ptree _pt;

    /*!
         * Parameter names and groups
         */
    std::map<std::string, std::vector<std::string> > _parameterGroupNameMapping;

    std::string filePath;

    /*!
         * Stores the wait time between two file read accesses
         */
    double _sleepDuration;

    /*!
         * Stores the local mutex to provide threadsafe container access
         */
    boost::mutex mx;

    /*!
         * If true reading is enabled
         */
    bool _read;

public:
    static Configuration& getInstance();
    virtual ~Configuration();

    /*!
         * Set the path where the config file is located
         * */
    inline void setPath(std::string file);

    /*!
         * Read the config file with a sleep duration of 1 second
         * */
    void startCyclicRead();

    /*!
         * Stops cyclic reading of the config file
         * */
    inline void stopCyclicRead(){
        boost::mutex::scoped_lock(mx);
        _read=false;
    }

    /*!
         * Get a parameter value from the config file
         * @return The first occurrence of the given parameter
         * */
    template <typename Type> Type getParameter(std::string groupName, std::string parameterName){
        // traverse pt
        using boost::property_tree::ptree;
        std::stringstream s;
        s<<groupName<<"."<<parameterName;

        boost::mutex::scoped_lock(mx);
        return  _pt.get<Type>(s.str());
    }

    /*!
         * Get the parameter list of the config file
         * @return The list of parameters
         * */
    std::vector<std::string> getParameterList(std::string groupName){
        auto iter = _parameterGroupNameMapping.find(groupName);
        if (iter != _parameterGroupNameMapping.end() )
            return iter->second;
        else
            UT_NOTIFY(LV_ERROR,"Parameter "<<groupName<<" group not found.");
        return std::vector<std::string>();
    }

private:
    Configuration();

    Configuration(Configuration const&);  // Don't Implement
    void operator=(Configuration const&); // Don't implement

    void read();
    inline void cyclic_read();

    /*!
         * Create the parameter list of the config file
         * */
    void createParameterList();
};

#ifdef OS_WIN
extern "C"
{
__declspec(dllexport) Configuration& getInstanceExtern();
__declspec(dllexport) void startCyclicReadExtern();
}
#endif
} /* namespace iMod */
#endif /* CONFIGURATION_H_ */
