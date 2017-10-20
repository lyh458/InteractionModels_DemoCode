#include "ObjTransform.h"

using namespace iModHaptic;

int ObjTransform::numberSphere=0;

 

 
 
ObjTransform::ObjTransform(){
	
	
 }
 
 
 ObjTransform::~ObjTransform(){
	 
 }
void ObjTransform::initialize(){
	
	for(int i=0;i<getnumberSphere(); i++){
	
		float n=0;
		bool m= false;
		X.push_back(n);
		Y.push_back(n);
		Z.push_back(n);
		R.push_back(n);
		X_rot.push_back(n);
		Y_rot.push_back(n);
		Z_rot.push_back(n);
		collidingObjects.push_back(m);
		for(int j=0; j<16; j++)
			Mat.push_back(n);
	}
		
}

std::vector<float> ObjTransform::getX(){

	return X;
	
}

void ObjTransform::setX(float x, int i){

	boost::mutex::scoped_lock(mx);

	X[i]=x;

}

std::vector<float> ObjTransform::getY(){

	return Y;
	
}

void ObjTransform::setY(float y, int i){

		boost::mutex::scoped_lock(mx);
	
	Y[i]=y;

	
}

std::vector<float> ObjTransform::getZ(){

	return Z;
	
}

void ObjTransform::setZ(float z, int i){

	boost::mutex::scoped_lock(mx);
	Z[i]=z;

}

std::vector<float> ObjTransform::getR(){
	

	return R;
	
}

void ObjTransform::setR(float r, int i){
		
		boost::mutex::scoped_lock(mx);
	

	R[i]=r;

	
}

std::vector<float> ObjTransform::getMat(){

	return Mat;
	
}

void ObjTransform::setMat(float mat[16], int i){

		boost::mutex::scoped_lock(mx);
		

	for(int j=0; j<16;j++)
			Mat[i*16+j]=mat[j];

	
}

std::vector<bool> ObjTransform::getcollidingObjects(){

	return collidingObjects;
	
}

void ObjTransform::setcollidingObjects(bool collidingobjects, int i){

		boost::mutex::scoped_lock(mx);

	collidingObjects[i]=collidingobjects;
	
}
int ObjTransform::getnumberSphere(){
		boost::mutex::scoped_lock(mx);
	
	return numberSphere;
}

void ObjTransform::setnumberSphere(int Zahl){

	boost::mutex::scoped_lock(mx);
	
	numberSphere=Zahl;

}





std::vector<float> ObjTransform::getX_rot(){

	return X_rot;
	
}

void ObjTransform::setX_rot(float x, int i){

	boost::mutex::scoped_lock(mx);

	X_rot[i]=x;

}

std::vector<float> ObjTransform::getY_rot(){

	return Y_rot;
	
}

void ObjTransform::setY_rot(float y, int i){

		boost::mutex::scoped_lock(mx);
	
	Y_rot[i]=y;

	
}

std::vector<float> ObjTransform::getZ_rot(){

	return Z_rot;
	
}

void ObjTransform::setZ_rot(float z, int i){

	boost::mutex::scoped_lock(mx);
	

	Z_rot[i]=z;

}
