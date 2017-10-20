//
//  MyInteractionMeshController.h
//  BoneAnimation
//
//  Created by Christian on 25.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionMeshController__
#define __BoneAnimation__MyInteractionMeshController__


#include <iostream> // std::cout, std::endl
#include <fstream> // std::ifstream
#include <string> // std::string
#include <exception> // std::exception
#include <vector>

#include <osgAnimation/Bone> // osgAnimation::Bone
#include "../utils/Configuration.h"

#include "MyInteractionMeshModel.h"
#include "MyInteractionMeshView.h"

#include "MyInteractionLineMesh.h"
#include "MyInteractionTriangleMesh.h"
#include "MyInteractionTetrahedronMesh.h"

#include "MyInteractionLineMeshView.h"
#include "MyInteractionTriangleMeshView.h"
#include "MyInteractionTetrahedronMeshView.h"

#include <osgGA/GUIEventHandler>

namespace MyInteractionMesh {
    typedef std::vector<MyInteractionMeshModel*> ModelVector;
    typedef std::vector<MyInteractionMeshView*> ViewVector;

/**
 * @brief Controller in the Model-View-Controller pattern of the MyInteractionMesh
 */
class MyInteractionMeshController {
    ModelVector _model;
    ViewVector  _view;
    MyMeshKind              _kindOfMesh;
    MyVerticeVisualisation  _kindOfVerticeVisualisation;
    
    
public:
    /**
     * @brief Constructor
     * @param kindOfMesh Which kind of Mesh should be generated
     * @param _root rootNode in which the InteractionMesh should be appended
     */
    MyInteractionMeshController(MyMeshKind kindOfMesh, osg::ref_ptr<osg::Group> _root=NULL);
    /**
     * @brief Deconstructor
     */
    virtual ~MyInteractionMeshController();
    /**
	 * @brief reads connections from a file and contructs a mesh from the given informations
	 * @param path the path to the file
	 * @param vh1Node the node where the model of virtual human 1 was added to
	 * @param vh2Node the node where the model of virtual human 2 was added to
	 */
    void readConnectionVectorFromFile(std::string path, std::vector<osg::Node*> nodeVector, std::vector<std::vector<std::string>> boneNameVector);
    
    /**
	 * @brief constructs the mesh from the information in the model
	 * @param inNode a pointer to the node where the mesh should be added to
	 */
    void addMeshToSceneGraph(osg::ref_ptr<osg::Group> inNode);
    
    /**
	 * @brief Updates the mesh to the current coords given by bones in model
	 */
    void updateMesh();
    
    /**
     * @brief Switches between the possible views of the interaction mesh
     */
    void switchMode();
    
    /**
     * @brief Switches between the possible visualisations for mesh vertices
     */
    void switchVerticeVisualisationMode();
    
    osg::ref_ptr<osg::Group> _root;   
private:
    /**
	 * @brief Adds a Line, given by 2 bones and their context information (which virtual human they belong to)
	 * @param bonesAndContext A tuple of 2 pairs of bones and MyInteractionModelAddingOption (or 0= toVH1, 1=toVH2)
	 */
    void addLine(BoneAndContext2Tuple bonesAndContext);
    /**
	 * @brief Adds a triangle, given by 3 bones and their context information (which virtual human they belong to)
	 * @param bonesAndContext A tuple of 3 pairs of bones and MyInteractionModelAddingOption (or 0= toVH1, 1=toVH2)
	 */
    void addTriangle(BoneAndContext3Tuple bonesAndContext);
    /**
	 * @brief Adds a tetrahedron, given by 4 bones and their context information (which virtual human they belong to)
	 * @param bonesAndContext A tuple of 4 pairs of bones and MyInteractionModelAddingOption (or 0= toVH1, 1=toVH2)
	 */
    void addTetrahedron(BoneAndContext4Tuple bonesAndContext);
    /**
	 * @brief Adds a triangle, given by 3 bones but without context information which VH they belong to (all are stored in 1 vertice vector)
	 * @param b1 a bone representing the first vertice of a triangle
     * @param b2 a bone representing the second vertice of a triangle
	 * @param b3 a bone representing the third vertice of a triangle
	 */
    void addTriangleWithoutContext(osgAnimation::Bone* b1, osgAnimation::Bone* b2, osgAnimation::Bone* b3);
    /**
	 * @brief Adds a Line, given by 2 bones but without context information which VH they belong to (all are stored in 1 vertice vector)
	 * @param b1 a bone representing the first vertice of a line
     * @param b2 a bone representing the second vertice of a line
	 */
    void addLineWithoutContext(osgAnimation::Bone* b1, osgAnimation::Bone* b2);
    /**
	 * @brief Adds a tetrahedron, given by 4 bones but without context information which VH they belong to (all are stored in 1 vertice vector)
	 * @param b1 a bone representing the first vertice of a tetrahedron
     * @param b2 a bone representing the second vertice of a tetrahedron
	 * @param b3 a bone representing the third vertice of a tetrahedron
     * @param b4 a bone representing the fourth vertice of a tetrahedron
	 */
    void addTetrahedronWithoutContext(osgAnimation::Bone* b1, osgAnimation::Bone* b2,osgAnimation::Bone* b3,osgAnimation::Bone* b4);
};
    
    /**
     * @brief Keyboardhandler for controlling MyInteractionMeshController via Keyboard
     * @param controller controller which should be controlled by Keyboard
     */
    class MyInteractionMeshKeyboardEventHandler: public osgGA::GUIEventHandler {
        
	protected:
        MyInteractionMeshController* _controller;
        
	public:
        MyInteractionMeshKeyboardEventHandler(MyInteractionMeshController* controller) {
            _controller = controller;
        }
        
        virtual bool handle(const osgGA::GUIEventAdapter& eAdapter, osgGA::GUIActionAdapter& aa, osg::Object* obj, osg::NodeVisitor* nv) {
            
            switch (eAdapter.getEventType()) {
                    
                case osgGA::GUIEventAdapter::KEYDOWN: {
                    switch (eAdapter.getKey()) {
                            
                        case osgGA::GUIEventAdapter::KEY_M:
                            _controller->switchMode();
                            return true;
                        case osgGA::GUIEventAdapter::KEY_V:
                            _controller->switchVerticeVisualisationMode();
                        default:
                            return false;
                    }
                }
            }
            return false;
        }
        
	};

    
}

#endif /* defined(__BoneAnimation__MyInteractionMeshController__) */
