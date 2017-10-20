#include "CollisionDetection.h"
#include <boost/thread.hpp>

using namespace iModHaptic;
int gGlobalValue = 42;
int obj_id1;
int obj_id2;
int j=0;

	std::vector<bool> CollisionDetection::collidingObjects = boost::assign::list_of(false);



bool callbackFunc(btManifoldPoint& cp,const btCollisionObjectWrapper* obj1, int id1, int index1,const btCollisionObjectWrapper* obj2, int id2, int index2){

			obj_id1=(((bulletObject*)obj1->getCollisionObject()->getUserPointer())->id);
			obj_id2=(((bulletObject*)obj2->getCollisionObject()->getUserPointer())->id);
			CollisionDetection::collidingObjects[obj_id1]=true;

			CollisionDetection::collidingObjects[obj_id2]=true;
			cp.m_userPersistentData = &gGlobalValue;


			return false;
	}




	bool ContactDestroyed_Callback(void* userPersistentData){


			btAssert( userPersistentData == &gGlobalValue){
				for(int i=0;i<CollisionDetection::collidingObjects.size();i++)
					CollisionDetection::collidingObjects[i]=false;


			}
			return false;

	}


CollisionDetection::CollisionDetection(){

		collisionConfig=new btDefaultCollisionConfiguration();
    	dispatcher=new btCollisionDispatcher(collisionConfig);
        broadphase=new btDbvtBroadphase();
        solver=new btSequentialImpulseConstraintSolver();
        world=new btDiscreteDynamicsWorld(dispatcher,broadphase,solver,collisionConfig);

		//




	}

CollisionDetection::~CollisionDetection(){


}

std::vector<bulletObject*>* CollisionDetection::getVector(){

	std::vector<bulletObject*>* _bodies;
	_bodies=&bodies;
	return _bodies;
}
 std::vector<bool> CollisionDetection::getCollidingObj(){


	return collidingObjects;
}

void CollisionDetection::createObject(int anzahl, int more_bodys){
	//Sphären für jede Marke
	for (int i=0; i<anzahl; i++){
		btRigidBody* body;
		_body=new CollisionBody();

		 body=_body->create_Sphere(0.06, 0, 0, 0, 1); /*!create Sphere for every target */

		world->addRigidBody(body);	/*!add every body to the world */

		int Sphere_id=i;
		bodies.push_back(new bulletObject(body,Sphere_id)); 	// put every body in a vector and add ID
		body->setUserPointer(bodies[bodies.size()-1]);

		/*!
		* set flags
		*  CF_CUSTOM_MATERIAL_CALLBACK: needed for callback
		*  CF_KINEMATIC_OBJECT: needed to change position of an object in the world
		*/
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT |btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );


	}

	//Zusätzliche Sphären/ Boxen
	for(int j=0; j<more_bodys;j++){
		btRigidBody* body;
		_body=new CollisionBody();

		body=_body->create_Sphere(0.06,0,0,0,1); // create additional sphere
		//body=_body->create_Box(3,0.01,3,0,2.1,0,1);// create Box
		//body=_body->create_Cylinder(0.04,0.5, 0, 0, 0, 1);// create cylinder

		world->addRigidBody(body);						// add Sphere into Bullets Dynamicsworld
		int Sphere_id=anzahl+j;
		bodies.push_back(new bulletObject(body,Sphere_id));
		body->setUserPointer(bodies[bodies.size()-1]);
		body->setCollisionFlags(body->getCollisionFlags() | btCollisionObject::CF_KINEMATIC_OBJECT |btCollisionObject::CF_CUSTOM_MATERIAL_CALLBACK );

	}

	gContactAddedCallback= callbackFunc; //  callback for collision
	gContactDestroyedCallback = ContactDestroyed_Callback; // callback when collision stops
	collidingObjects.resize(anzahl+more_bodys, false); // size of vector is equal to number of rigidbodys

}


void CollisionDetection::startSimulation(){

				world->stepSimulation(1.0f/2000.0f,0); // Start Bullet Stepsimulation for collision detection





}
