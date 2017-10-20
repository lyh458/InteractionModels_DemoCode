#include "notify.h"
#include "Configuration.h"

int main(){
	UT_NOTIFY(LV_INFO,"Logging Test "<<" "<<1);
    UT_NOTIFY(LV_DEBUG,"Logging Test "<<" "<<1);
    UT_NOTIFY(LV_FATAL,"Logging Test "<<" "<<1);
    UT_NOTIFY(LV_ERROR,"Logging Test "<<" "<<1);
    UT_NOTIFY(LV_TRACE,"Logging Test "<<" "<<1);
    UT_NOTIFY(LV_WARN,"Logging Test "<<" "<<1);
    
    
    iMod::Configuration::getInstance().startCyclicRead();
    std::vector<std::string> parameterList = iMod::Configuration::getInstance().getParameterList("ART");
    
    UT_NOTIFY(LV_INFO, "Parameters of ART in the configuration file");
    for(const auto j : parameterList)
         UT_NOTIFY(LV_INFO, j);
    
}
