#ifndef BULLETOBJECT_H_
#define BULLETOBJECT_H_
#include <btBulletDynamicsCommon.h>
namespace iModHaptic {
	
class bulletObject {

	public:
	int id;
   
        btRigidBody* body;
	bulletObject( btRigidBody* b,int i);
	virtual ~bulletObject();	
		
};


}
#endif /* BULLETOBJECT_H_ */
