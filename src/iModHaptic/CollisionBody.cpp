#include "CollisionBody.h"


using namespace iModHaptic;

CollisionBody::CollisionBody(){
	
		
}


CollisionBody::~CollisionBody(){
	
	
}


btRigidBody* CollisionBody::create_Sphere(float rad, float x,float y, float z, float mass){
	boost::mutex::scoped_lock(mx);

		
			btTransform t;
			t.setIdentity();
			t.setOrigin(btVector3(x,y,z));
			btSphereShape* sphere=new btSphereShape(rad);
			btVector3 inertia(0,0,0);
			if(mass!=0.0)
					sphere->calculateLocalInertia(mass,inertia);
			btMotionState* motion=new btDefaultMotionState(t);
			btRigidBody::btRigidBodyConstructionInfo info(mass,motion,sphere,inertia);
			btRigidBody* Sphere=new btRigidBody(info);
			 
			
			return Sphere;

}

btRigidBody* CollisionBody::create_Cylinder(float r,float h,float x,float y,float z,float mass)
{
        btTransform t;
        t.setIdentity();
        t.setOrigin(btVector3(x,y,z));
        btCylinderShape* Cylinder=new btCylinderShape(btVector3(r,h/2.0,r));
        btVector3 inertia(0,0,0);
        if(mass!=0.0)
                Cylinder->calculateLocalInertia(mass,inertia);
       
        btMotionState* motion=new btDefaultMotionState(t);
        btRigidBody::btRigidBodyConstructionInfo info(mass,motion,Cylinder,inertia);
        btRigidBody* cylinder=new btRigidBody(info);
       
        return cylinder;
}

btRigidBody* CollisionBody::create_Plane(){

        btTransform t;
        t.setIdentity();
        btStaticPlaneShape* Plane=new btStaticPlaneShape(btVector3(0,-1,0),0);
      
        btMotionState* motion=new btDefaultMotionState(btTransform(btQuaternion(0,0,0,1),btVector3(0,15,0)));
        btRigidBody::btRigidBodyConstructionInfo info(0.0,motion,Plane);

        btRigidBody* plane=new btRigidBody(info);
		return plane;
      
}

btRigidBody* CollisionBody::create_Box(float width,float height,float depth,float x,float y,float z,float mass)
{
        btTransform t;
        t.setIdentity();
        t.setOrigin(btVector3(x,y,z));
        btBoxShape* Box=new btBoxShape(btVector3(width/2.0,height/2.0,depth/2.0));
        btVector3 inertia(0,0,0);
        if(mass!=0.0)
                Box->calculateLocalInertia(mass,inertia);
       
        btMotionState* motion=new btDefaultMotionState(t);
        btRigidBody::btRigidBodyConstructionInfo info(mass,motion,Box,inertia);
        btRigidBody* box=new btRigidBody(info);
        
        return box;
}

