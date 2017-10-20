#ifndef OBJTRANSFORM_H_
#define OBJTRANSFORM_H_

#include <iostream>
#include <vector>
#include <boost/thread.hpp>
#include <boost/assign/list_of.hpp>
//#DEFINE ASD 16
namespace iModHaptic {
	
	

	class ObjTransform{
	boost::mutex mx;
	public:	
	
	 std::vector<float> X;
	 std::vector<float> Y;
	 std::vector<float> Z;
	 std::vector<float> X_rot;
	 std::vector<float> Y_rot;
	 std::vector<float> Z_rot;
	 std::vector<float> R;
	 std::vector<float> Mat;
	 std::vector<bool> collidingObjects;
	static int numberSphere;
			
	//vector.resize(zahl, 0)
	
	public:
	
	ObjTransform();
	virtual ~ObjTransform();	
	void initialize();
		
	std::vector<float> getX();
	void setX(float x, int i);
	
	std::vector<float> getY();
	void setY(float y, int i);
	
	std::vector<float> getZ();
	void setZ(float z, int i);
	
	std::vector<float> getR();
	void setR(float r, int i);
	
	std::vector<float> getMat();
	void setMat(float mat[16], int i);
	
	std::vector<bool> getcollidingObjects();
	void setcollidingObjects(bool collidingbjects, int i);
	
	
	int getnumberSphere();
	void setnumberSphere(int Zahl);
	
	std::vector<float> getX_rot();
	void setX_rot(float x, int i);
	std::vector<float> getY_rot();
	void setY_rot(float y, int i);
	std::vector<float> getZ_rot();
	void setZ_rot(float z, int i);
	
	
	};
}
#endif /* OBJTRANSFORM_H_ */
