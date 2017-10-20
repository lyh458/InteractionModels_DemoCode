#include <btBulletDynamicsCommon.h>

#include <boost/thread.hpp>
#include <iostream>

namespace iModHaptic {
	

/*	  struct bulletObject{
        int id;
   
        btRigidBody* body;
        bulletObject(btRigidBody* b,int i) : body(b),id(i) {}
	};*/
	 
	class CollisionBody{
		
	
	boost::mutex mx;
	
	public:

	CollisionBody();
	virtual ~CollisionBody();	
	
	/*!create different Rigidbodys */	
	btRigidBody* create_Sphere(float rad, float x,float y, float z, float mass);
	btRigidBody* create_Cylinder(float d,float h,float x,float y,float z,float mass);
	btRigidBody* create_Plane();
	btRigidBody* create_Box(float width,float height,float depth,float x,float y,float z,float mass);
	};
	
	
	
	
	
}

