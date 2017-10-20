#include "Visual.h"


using namespace iModHaptic;


		


Visual::Visual(  ObjTransform* objtransform){

	_objtransform=objtransform;

	
	
	
}

Visual::~Visual(){
	
	

	
}


void Visual::run(float angle,std::vector<bulletObject*> bodies){
				
				float n=0;
				bool m=false;
				
					/*!
	 * set size of vectors, is equivalent with the number of rigidbodys
	 */
				for(int a=0;a< _objtransform->getnumberSphere();a++){
				r.push_back(n);
				x.push_back(n);
				y.push_back(n);
				z.push_back(n);
				x_rot.push_back(n);
				y_rot.push_back(n);
				z_rot.push_back(n);
				hit.push_back(m);
				
	/*!
	 * 16 values for each object
	 */	
				
				for(int b=0; b<16; b++)
					_mat.push_back(n);
	
			}
				
				/*!
				* init Display
				*/
				SDL_Init(SDL_INIT_EVERYTHING);
				SDL_SetVideoMode(600,480,32,SDL_OPENGL);
				glClearColor(0,0,0,1);
				glMatrixMode(GL_PROJECTION);
				glLoadIdentity();
				gluPerspective(angle,640.0/480.0,-10,0);
				glMatrixMode(GL_MODELVIEW);
				quad=gluNewQuadric();
					
				glEnable(GL_DEPTH_TEST);
			
				cam.setLocation(vector3d(0,-3,0));
				cam.lookAt(79.7,2.8);
				//cam.setLocation(vector3d(0,10,0));
				//cam.lookAt(90,0);
				
				std::vector<bulletObject*> body=bodies;
			
				Display( Event, body);
				
	}
	
	


void Visual::Display( SDL_Event event,std::vector<bulletObject*> bodies){

			while(1)
			{
		
				
				
				
				glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

				glLoadIdentity();
				cam.Control();
			
				cam.UpdateCamera();
				
				
		
				/*!
				* render each objekt
				*/	
				for(int i=0; i<_objtransform->getnumberSphere();i++){
						if(bodies[i]->body->getCollisionShape()->getShapeType()==SPHERE_SHAPE_PROXYTYPE)
							renderSphere(i);
						/*else if(bodies[i]->body->getCollisionShape()->getShapeType()==CYLINDER_SHAPE_PROXYTYPE){
							btVector3 extent=((btCylinderShape*)bodies[i]->body->getCollisionShape())->getHalfExtentsWithoutMargin()*2;
							renderCylinder(i, extent);
                        }*/
                        else if(bodies[i]->body->getCollisionShape()->getShapeType()==BOX_SHAPE_PROXYTYPE){
						btVector3 extent=((btBoxShape*)bodies[i]->body->getCollisionShape())->getHalfExtentsWithoutMargin()*2;
                        renderBox(i, extent);
                       }
                       
                      // else if(bodies[i]->body->getCollisionShape()->getShapeType()==STATIC_PLANE_PROXYTYPE)
							//renderPlane(i);
					
						
				}
					
				
				SDL_GL_SwapBuffers();
				if(1000.0/60>SDL_GetTicks()-start)
					SDL_Delay(1000.0/60-(SDL_GetTicks()-start));
				start=SDL_GetTicks();  
				while(SDL_PollEvent(&event)){
				
						switch(event.type){
								case SDL_QUIT:							//close only the visual Thread, not the whole program
										std::cout<<"quit"<<std::endl;
										running=false;
										stopDisplay();
										break;
								case SDL_KEYDOWN:
										switch(event.key.keysym.sym){
												case SDLK_ESCAPE:		// key escape stops this Thread
														running=false;
														break;
												case SDLK_y:
												
														cam.mouseIn(false); // mouse can be used out of the display
														break;
												}
												break;
										case SDL_KEYUP:
												switch(event.key.keysym.sym){
														}
												break;                 
										case SDL_MOUSEBUTTONDOWN:
												cam.mouseIn(true);
											
															//mouse can only used in the display 
												break;
                        
               
						}
				}	

				
                   
			}
		
        
	
}

void Visual::stopDisplay(){
		SDL_Quit();
		gluDeleteQuadric(quad);
}
bool Visual::stopprogramm(){
	return running;
}
	void Visual::renderSphere( int k){ 
	
		
		r[k]=_objtransform->getR()[k];  				/*! stores radius of each sphere in vektor r */
		
		
		
		hit[k]= _objtransform->getcollidingObjects()[k];  /*! stores bool value for collision in vector hit */
	
		if(!hit[k])
				glColor3f(1,0,0);  /*! set color */
				
        else
                glColor3f(0,1,0);
       
	
		for(int j=0;j<16;j++)
				mat[j]=_objtransform->getMat()[k*16+j];  /*! stores position and rotation in vektor mat */
				
		
		glPushMatrix();
		
		glMultMatrixf(mat);	/*!multiplying the current matrix with it moves the object in place */
			
	
		gluSphere(quad,r[k],20,20);
		glPopMatrix();
		
			
	}
	/*void Visual::renderCylinder( int k,btVector3 extent){ 
		
		r[k]=_objtransform->getR()[k];
		 //r[k]=_r[k];
		

		
		
		hit[k]= _objtransform->getcollidingObjects()[k];
		//std::cout<<hit[k]<<std::endl;
		if(!hit[k])
		glColor3f(1,0,0);
				
				
        else
                glColor3f(0,1,0);
       
		
		//_mat=*_objtransform->getMat();
		for(int j=0;j<16;j++)
				mat[j]=_objtransform->getMat()[k*16+j];
				
	
		glPushMatrix();
				glMultMatrixf(mat);     //translation,rotation
			//	glScalef(-1,1,-1);
              // glTranslatef(0,extent.y(),0);
             //  glRotatef(90,0,1,1);
                gluCylinder(quad,extent.x(),extent.x(),extent.y(),20,20);

		glPopMatrix();
		
		
}

void Visual::renderPlane(int k)
{
      
   
                
        hit[k]= _objtransform->getcollidingObjects()[k];        
      if(!hit[k])
				glColor3f(1,0.3,0.1);
				
        else
                glColor3f(0,1,0);
       
       
       for(int j=0;j<16;j++)
				mat[j]=_objtransform->getMat()[k*16+j];
       
        glPushMatrix();
                glMultMatrixf(mat);     //translation,rotation
                 
                glBegin(GL_QUADS);
                      glVertex3f(1.5,-0.64,-10);
                        glVertex3f(1.5,-0.64,10);
                        glVertex3f(	1.5,2.5,-10);
                        glVertex3f(1.5,2.5,10);
                glEnd();
        glPopMatrix();
}*/

void Visual::renderBox(int k,btVector3 extent)
{
		
       	hit[k]= _objtransform->getcollidingObjects()[k];
		//std::cout<<hit[k]<<std::endl;
		if(!hit[k])
				glColor3f(0,0.1,1);	
        else
                glColor3f(0,1,0);
        
       
        for(int j=0;j<16;j++)
				mat[j]=_objtransform->getMat()[k*16+j];
				
       
        glPushMatrix();
                glMultMatrixf(mat);     //translation,rotation
                glScalef(1,-1,1);
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());         
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(extent.x(),extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());          
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());          
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());         
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),extent.y(),-extent.z());
                        glVertex3f(-extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),extent.z());
                        glVertex3f(extent.x(),extent.y(),-extent.z());         
                glEnd();
                glBegin(GL_QUADS);
                        glVertex3f(-extent.x(),-extent.y(),-extent.z());
                        glVertex3f(-extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),extent.z());
                        glVertex3f(extent.x(),-extent.y(),-extent.z());        
                glEnd();               
        glPopMatrix();
}
 
