#include "../interactionMesh/BlenderController.h"
#include "../interactionMesh/PoseContainer.h"
#include "../utils/Configuration.h"


#include <iostream>
#include <string>
#include <boost/thread.hpp>



namespace iModHaptic {
	

	
	class BlenderController{
	
		boost::mutex mx;
		int i;
	
	public:
	iMod::BlenderController* _blendercontroller;
	BlenderController();
	virtual ~BlenderController();	

	/*!
	 * return the last sendet string
	 */
	iMod::Pose_t getPosition();
	
	/*!
	 * Initialize the connection to Blender
	 */
	void init();
	

	
		
	
	
	};
}
