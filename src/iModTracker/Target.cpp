/*
 * Target.cpp
 *
 *  Created on: Sep 24, 2013
 *      Author: ddvogt
 */

#include "Target.h"

namespace iModTracker {

Target::Target() {
	// TODO Auto-generated constructor stub
	pos = new double[3];
	rot = new double[9];

	pos[0] = 0;
	pos[1] = 0;
	pos[2] = 0;

	rot[0] = 0;
	rot[1] = 0;
	rot[2] = 0;

	rot[3] = 0;
	rot[4] = 0;
	rot[5] = 0;

	rot[6] = 0;
	rot[7] = 0;
	rot[8] = 0;

	name = "notSet";
}

Target::~Target() {
	// TODO Auto-generated destructor stub
	//delete pos;
	//delete rot;
}


Target::Target(const Target& cp):
	pos(cp.pos),
	rot(cp.rot),
	name(cp.name)
{
	
}


std::ostream& operator << ( std::ostream& out, const std::string& bs )
{
    out << bs.c_str();
    return out;
}

std::ostream& operator<< (std::ostream &out, Target& t){
    out<<t.getName()<<" "<<t.getPosition()[0]<<" "<<t.getPosition()[1]<<" "<<t.getPosition()[2];
	  // <<t.rot[0]<<" "<<t.rot[1]<<" "<<t.rot[2];
	return out;
}

} /* namespace iModTracker */
