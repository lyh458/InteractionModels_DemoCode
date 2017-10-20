#include "../utils/Configuration.h"
#include "../utils/notify.h"
#include "camera.h"
#include "ObjTransform.h"
#include "BulletObject.h"


#include <SDL/SDL.h>
#include <GL/gl.h>
#include <GL/glu.h>
#include <iostream>
#include <boost/assign/list_of.hpp>

namespace iModHaptic {

	class Visual{
		
	 std::vector<float> r; 
	 std::vector<float> x;
	 std::vector<float> y;
	 std::vector<float> z;
	 std::vector<float> x_rot;
	 std::vector<float> y_rot;
	 std::vector<float> z_rot;
	 std::vector<bool> hit;
	 std::vector<float> _mat;
	float mat[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
	
	Uint32 start;
	SDL_Event Event;
	camera cam;
	GLUquadricObj* quad;
	
	ObjTransform* _objtransform;		
	bool running=true;	
			
	public:
	
	
	Visual(ObjTransform* objtransform);
	virtual ~Visual();	
	
	/*!
	 * create Display
	 * @param angle Field of View angle in y direction
	 * @param bodies vector of rigidbodys
	 */
	void run(float angle, std::vector<bulletObject*> bodies);
	
	/*!
	 * render rigidbodys
	 * @param event click in the Display to navigate, stop program...
	 * @param bodies vector of rigidbodys
	 */
	void Display( SDL_Event event,std::vector<bulletObject*> bodies);
	
	/*!
	 * render Sphere
	 * @param k ID from Sphere
	 */
	
	void renderSphere( int k);
	
		/*!
	 * render cylinder
	 * @param k ID from cylinder
	 * @param extent size of the cylinder
	 */
	//void renderCylinder( int k,btVector3 extent);
	
	/*!
	 * render plane
	 * @param k ID of the plane
	 */
	//void renderPlane( int k);
	
		/*!
	 * render box
	 * @param k ID from box
	 * @param extent size of the box
	 */
	void renderBox( int k, btVector3 extent);
	
		/*!
	 * close display after program finished
	 */
	void stopDisplay();
	
		/*!
	 * return bool value to stop HapticFeedbackcontroller 
	 */
	bool stopprogramm();
	};
	
	
	
	
	
}
