

#ifndef HAPTICFEEDBACKCONTROLLER_H_
#define HAPTICFEEDBACKCONTROLLER_H_


#include <btBulletDynamicsCommon.h>
#include "Visual.h"
#include "CollisionDetection.h"
#include "Wii.h"
#include "Udp.h"
#include "HapticFeedback.h"
#include "BlenderController.h"

#include <string>
#include <iostream>

#include "boost/date_time/posix_time/posix_time.hpp"

namespace iModHaptic {

	
	

	class HapticFeedbackController{
	bool links	;	
	bool rechts;
	float angle=60;
	
	std::vector<bool> collidingObjects;
	HapticFeedback* _udp;
    Visual* _visual;
    BlenderController* _blenderController;
    HapticFeedback* _wiicontroller;
	ObjTransform* _objtransform;
	CollisionDetection* _detection;
	std::vector<bulletObject*> bodies;
	public:

  

	public:
	HapticFeedbackController();
	virtual ~HapticFeedbackController();	
	 
				
	
	void initCollisionResponse();
	void initPhysics();
	void initVisual();
	void initBlenderController();

	

	};


typedef std::shared_ptr<HapticFeedbackController> BRTPtr;
}
extern "C"
{
	extern void MC_InitFunc(void) __attribute__ ((constructor));
	extern void MC_TermFunc(void) __attribute__ ((destructor));

	static iModHaptic::HapticFeedbackController* bullet;

	//! Library Constructor
	void MC_InitFunc(void);

	//! Destructor
	void MC_TermFunc(void);
	
	void MC_initCollisionResponse();
	void MC_initPhysics();
	void MC_initBlenderController();
	void MC_initVisual();
	

}

#endif /* HAPTICFEEDBACKCONTROLLER_H_ */
