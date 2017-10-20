/*
 * MessageConverter.cpp
 *
 *  Created on: May 8, 2011
 *      Author: comander
 */

#include <sstream>
#include <stdexcept>
#include "MessageConverter.h"
#include <assert.h>
 #include "../utils/notify.h"

using namespace iMod;

MessageConverter::MessageConverter() {
	// TODO Auto-generated constructor stub
}

MessageConverter::~MessageConverter() {
	// TODO Auto-generated destructor stub
}

void split(const std::string &s, char delim, std::vector<double> &elems) {
	std::stringstream ss(s);
	std::string item;
	int i = 0;
	while (std::getline(ss, item, delim)) {
		try {
			elems.at(i++) = atof(item.c_str());
		} catch (std::out_of_range &e) {
			UT_NOTIFY(LV_ERROR,e.what());
		}
	}
}

void MessageConverter::convertToDouble(const char* message, std::vector<double> &data) {
	std::stringstream ss;
	ss << message;
	split(ss.str(), ' ', data);
}

std::string MessageConverter::convertToString(std::vector<double> data) {
	try {
		std::stringstream s;
		for (unsigned int i = 0; i < data.size(); i++) {
			char szWord[8] = "";
			double value = data[i];
			sprintf(szWord, "%1.4f", value);
			s << szWord << ",";
		}
		s << "\n";
		return s.str();
	} catch (const std::out_of_range &ex) {
		UT_NOTIFY(LV_ERROR,ex.what());
	}
	return "";
}
