/*
 * MessageConverter.h
 *
 *  Created on: May 8, 2011
 *      Author: David Vogt
 */

#ifndef MESSAGECONVERTER_H_
#define MESSAGECONVERTER_H_

#include <vector>
#include <string>

namespace iMod{

/*!
 * A simple message converter class.
 * */
class MessageConverter {
public:
	MessageConverter();
	virtual ~MessageConverter();

	void convertToDouble(const char* message, std::vector<double> &data);
	std::string convertToString(std::vector<double> data);
};
}
#endif /* MESSAGECONVERTER_H_ */
