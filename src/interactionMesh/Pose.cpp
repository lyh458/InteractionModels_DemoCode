/*
 * Pose.cpp
 *
 *  Created on: Mar 8, 2013
 *      Author: ddvogt
 */
#include "Pose.h"

std::ostream& iMod::operator <<(std::ostream& os, Pose_t &p){
		os << "[";
		for (std::vector<double>::iterator ii = p.begin(); ii != p.end(); ++ii)
			os << " " << *ii;

		os << " ]";
		return os;
}
