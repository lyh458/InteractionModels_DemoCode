#include "HapticFeedbackController.h"


using namespace iModHaptic;

HapticFeedbackController::HapticFeedbackController(){


	_blenderController=new BlenderController();
	_detection=new CollisionDetection();
	_wiicontroller=new Wii();
	_objtransform=new ObjTransform();
	 _udp=new Udp();
	 _visual=new Visual(_objtransform);

}

HapticFeedbackController::~HapticFeedbackController(){







}


void HapticFeedbackController::initCollisionResponse(){

	_udp->init();
	_wiicontroller->init();





}
void HapticFeedbackController::initPhysics(){


	std::cout<<"wait for number of spheres"<<std::endl;

	/*!wait until first string from Blender is received */
	while(_blenderController->_blendercontroller->poseContainer->isContainerFilled()==false){

	}

	/*!get number of targets   */
	iMod::Pose_t actualpose=_blenderController->getPosition();
	int anzahl=actualpose[0];
	int more_sphere=0; /*!set number of additional objects */

	std::cout<<anzahl<<std::endl;

	/*!resize all vectors in class Objtransform to number of all objects*/
	_objtransform->setnumberSphere(anzahl+more_sphere);
	_objtransform->initialize();


	/*!create objects and add them to physicworld */
	_detection->createObject(anzahl, more_sphere);

	/*!get vector with all rigidbodys */
	bodies=*_detection->getVector();

	float mat[16]={0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
		int j=0;

		/*!set values in every vector in class ObjTransform */
	 auto physBodies = *_detection->getVector();

		for (auto i:physBodies){
		btRigidBody* body=_detection->bodies[j]->body;
			_objtransform->setX(0, j);
			_objtransform->setY(0, j);
			_objtransform->setZ(0, j);
			_objtransform->setX_rot(0, j);
			_objtransform->setY_rot(0, j);
			_objtransform->setZ_rot(0, j);
			_objtransform->setR(1, j);
			_objtransform->setMat(mat, j);
			_objtransform->setcollidingObjects(false, j);



			j++;
		}



	/*!start visualization */

	std::string Haptic = iMod::Configuration::getInstance().getParameter<std::string>("Haptic","DRAWGui");
	if(Haptic=="1")   //if parameter DrawGui in configuratin.ini is "1" -> show display
				boost::thread initVisual(boost::bind(&Visual::run, _visual, angle, physBodies));






	bool running=true;

	while(running){



		/*!wait for next String from Blender */

		 while(_blenderController->_blendercontroller->poseContainer->isContainerFilled()==false){

		}


		/*!load bool vector collidingObjects */

		collidingObjects=_detection->getCollidingObj();

		int pose=0; /*!used as Position in the Sting sendet from Blender */
		int number=0; /*!is equal to ID of the rigidbodys */


				/*!last sendet String */
				iMod::Pose_t actualPose=_blenderController->getPosition();

				auto physBodies = *_detection->getVector();
				float mat[16];


				for (auto i:physBodies){

						/*!get Radius from every Sphere*/
						float r = ((btSphereShape*)i->body->getCollisionShape())->getRadius();

						/*!read coordinates from String*/
						float x = actualPose[pose];
						float y = actualPose[pose+1];
						float z = actualPose[pose+2];

						/*!set values to vector in class ObjTransform */
						_objtransform->setX(x, number);

						_objtransform->setY(y, number);

						_objtransform->setZ(z, number);

						_objtransform->setR(r, number);

						btTransform t;

						t.setIdentity();

						/*!change Position of the Rigidbody */
						t.setOrigin(btVector3(x,y,z));

					/*!read position and rotation and write the values in vector mat*/
					i->body->getMotionState()->setWorldTransform(t);
					t.getOpenGLMatrix(mat);

					/*!set Position and rotation to vector in class ObjTransform */
					_objtransform->setMat(mat, number);



			//Unterarme
			/*!number==16 wenn alle 16 targets aktiv sind, sonst muss der Wert anders gewählt werden */
				/*if (number==16){


					float x=(actualPose[33]-actualPose[39]); // Richtungsvektor von Ellenbogen und Handmarke
					float y=(actualPose[34]-actualPose[40]);
					float z=(actualPose[35]-actualPose[41]);
								btTransform t;
					x=actualPose[33]-0.5*x;  //Position Ellenbogenmarkne und verschiebe
					y=actualPose[34]-0.5*y;
					z=actualPose[35]-0.5*z;
						t.setIdentity();
						t.setOrigin(btVector3(x,y,z));

						i->body->getMotionState()->setWorldTransform(t);
						t.getOpenGLMatrix(mat);
						_objtransform->setMat(mat, number);

						_objtransform->setcollidingObjects(collidingObjects[number], number);

					}
					if (number==17){


					float x=(actualPose[30]-actualPose[36]);
					float y=(actualPose[31]-actualPose[37]);
					float z=(actualPose[32]-actualPose[38]);
								btTransform t;
					x=actualPose[30]-0.5*x;
					y=actualPose[31]-0.5*y;
					z=actualPose[32]-0.5*z;
						t.setIdentity();
						t.setOrigin(btVector3(x,y,z));

						i->body->getMotionState()->setWorldTransform(t);
						t.getOpenGLMatrix(mat);
						_objtransform->setMat(mat, number);

						_objtransform->setcollidingObjects(collidingObjects[number], number);

					}

					if (number==18){ //linke Arm


					float x=(actualPose[6]-actualPose[12]);
					float y=(actualPose[7]-actualPose[13]);
					float z=(actualPose[8]-actualPose[14]);
								btTransform t;
					x=actualPose[6]-0.5*x;
					y=actualPose[7]-0.5*y;
					z=actualPose[8]-0.5*z;
						t.setIdentity();
						t.setOrigin(btVector3(x,y,z));

						i->body->getMotionState()->setWorldTransform(t);
						t.getOpenGLMatrix(mat);
						_objtransform->setMat(mat, number);

						_objtransform->setcollidingObjects(collidingObjects[number], number);



					}

					if (number==19){


					float x=(actualPose[9]-actualPose[15]);
					float y=(actualPose[10]-actualPose[16]);
					float z=(actualPose[11]-actualPose[17]);
								btTransform t;
					x=actualPose[9]-0.5*x;
					y=actualPose[10]-0.5*y;
					z=actualPose[11]-0.5*z;
						t.setIdentity();
						t.setOrigin(btVector3(x,y,z));

						i->body->getMotionState()->setWorldTransform(t);
						t.getOpenGLMatrix(mat);
						_objtransform->setMat(mat, number);

						_objtransform->setcollidingObjects(collidingObjects[number], number);



					}*/


			// CAVE-Wände Box-shape
				/*!Hier waren nur 8 Marken aktiv */
				/*if(number==8){



						//x=actualPose[96];
						 btTransform t;
						 t.setIdentity();
						 i->body->getMotionState()->getWorldTransform(t);
						 t.setOrigin(btVector3(-1.505,0,0));


						i->body->getMotionState()->setWorldTransform(t);
						t.getOpenGLMatrix(mat);
						_objtransform->setMat(mat, number);
						_objtransform->setcollidingObjects(collidingObjects[number], number);


					}
					if(number==9){



						//x=actualPose[96];
						 btTransform t;
						 t.setIdentity();
						 i->body->getMotionState()->getWorldTransform(t);
						 t.setOrigin(btVector3(1.505,0,0));


						i->body->getMotionState()->setWorldTransform(t);
						t.getOpenGLMatrix(mat);
						_objtransform->setMat(mat, number);
						_objtransform->setcollidingObjects(collidingObjects[number], number);


					}
						if(number==10){



						//x=actualPose[96];
						 btTransform t;
						 t.setIdentity();
						 i->body->getMotionState()->getWorldTransform(t);
						 t.setOrigin(btVector3(0,2.1,0));


						i->body->getMotionState()->setWorldTransform(t);
						t.getOpenGLMatrix(mat);
						_objtransform->setMat(mat, number);
						_objtransform->setcollidingObjects(collidingObjects[number], number);


					}*/
					pose=pose+3;

					number=number+1;


				}



				//verändern des Radius einer Sphäre, wenn A oder B gedrückt wurde (Wii-Controller)
				/*	float j = ((btSphereShape*)physBodies[5]->body->getCollisionShape())->getRadius();
					((btSphereShape*)physBodies[5]->body->getCollisionShape())->setUnscaledRadius(j+_wiicontroller->is_pressed()); //Radius + zurückgegebener Wert
					std::cout<<j<<std::endl;*/

				/*if (collidingObjects[4]==false) // Boolwert nach letzter Simulation für Handmarken
					 links=false;
				else
					 links=true;

				if(collidingObjects[5]==false)
					 rechts=false;
				else
					 rechts=true;   */

					//start simulation
				_detection->startSimulation();

				/*!get vector with bool value after the callbacks */
				collidingObjects=_detection->getCollidingObj();
				int a=0;

				/*!set the bool values to the vector in class ObjTransform */
				for (auto i:physBodies){

					_objtransform->setcollidingObjects(collidingObjects[a], a);
					a++;
				}



				/*!generate Feedback*/
				if( collidingObjects[4]==true){

						//_udp->startFeedbackLeftThumb();
						//_udp->startFeedbackLeftIndex();
						_udp->startFeedbackLeftMiddle();

					_wiicontroller->startFeedbackLeft();

				}

				/*!Feedback wird nur gestoppt, wenn es im letzten durchlauf aktiviert war */
				else{

					//if(links){
					//_udp->stoppFeedbackLeftThumb();
					//_udp->stoppFeedbackLeftIndex();

					_udp->stoppFeedbackLeftMiddle();

					_wiicontroller->stoppFeedbackLeft();
					//}
				}

				if ( collidingObjects[5]==true){


					_wiicontroller->startFeedbackRight();
					//_udp->startFeedbackRightThumb();
					//_udp->startFeedbackRightIndex();

					_udp->startFeedbackRightMiddle();


				}
				else{

					//if(rechts){
					_wiicontroller->stoppFeedbackRight();
					//_udp->stoppFeedbackRightThumb();
					//_udp->stoppFeedbackRightIndex();

					_udp->stoppFeedbackRightMiddle();
					//}
				}


				/*!wenn der Thread visual gestoppt wird, endet auch dieser nach der nächsten Schleife */
				if(Haptic=="1")
				running =_visual->stopprogramm();
	}



	std::cout<<"quit"<<std::endl;
}

void HapticFeedbackController::initVisual(){

		//_visual->run(angle);

}

void HapticFeedbackController::initBlenderController(){


	_blenderController->init();



}
