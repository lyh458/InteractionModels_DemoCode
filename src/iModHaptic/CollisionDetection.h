#include <btBulletDynamicsCommon.h>
#include "CollisionBody.h"
#include "ObjTransform.h"
#include "BulletObject.h"

#include <boost/assign/list_of.hpp>
#include <iostream>

namespace iModHaptic {
	
	
	class CollisionDetection{
	
	CollisionBody* _body;
	btCollisionConfiguration* collisionConfig;
	btDispatcher* dispatcher;
	btBroadphaseInterface* broadphase;
	btConstraintSolver*  solver;
	
	public:
		static std::vector<bool> collidingObjects; 
		std::vector<bulletObject*> bodies;
	btDynamicsWorld*  world; 	/*! start the physicworld*/

	public:
			
		CollisionDetection();
		virtual ~CollisionDetection();
	
		
			/*!return vector of rigidbodys */		
		std::vector<bulletObject*>* getVector();
		
			/*!return vector with bool values to generate Feedback */	
		 std::vector<bool> getCollidingObj();
		 
		 	/*!add all rigidbodys to physikworld
		 	* @param anzahl total number of all aktive targets
			* @param more_bodys number of additional rigidbodys
			*/	
		void createObject(int anzahl, int more_bodys);
		
		/*!start simulation in the physicsworld */	
		void startSimulation();

 
	};
}
