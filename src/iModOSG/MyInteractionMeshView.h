//
//  MyInteractionMeshView.h
//  BoneAnimation
//
//  Created by Christian on 25.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionMeshView__
#define __BoneAnimation__MyInteractionMeshView__

#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/Shape>
#include <osg/ShapeDrawable>
#include <osg/Geode>
#include <osg/LineWidth>

#include <osg/Material>
#include <osg/BlendFunc>
#include <exception> // std::exception
#include <vector> // std::vector
#include "MyInteractionMeshModel.h"

// index = index of triangle in models connectionVector
// Geode = drawable which holds the visible triangle

namespace MyInteractionMesh {
    
typedef std::vector<osg::Geode*> GeodeVector;

enum MyVerticeVisualisation {
    Off = 0,
    Sphere = 1,
    Axis = 2
};
    
/**
 * @brief View in the Model-View-Controller pattern of the MyInteractionMesh
 */
class MyInteractionMeshView {

protected:
    MyInteractionMeshModel*  _model;
    GeodeVector              _geodeVector;
    GeodeVector              _verticeVizualitionVector;     // Holds Geodes which visualize the positition of vertices in the model
    osg::ref_ptr<osg::Group> _rootNode;
    
public:
    MyInteractionMeshView();
    virtual ~MyInteractionMeshView();
    /**
     * @brief Goes through the Model and adds a Geode for each Shape stored in the Model to a Node in OSG
     * 
     * @param inNode Node to which the InteractionMesh should be appended to
     */
    void compileMeshAndAddToSceneGraph(osg::ref_ptr<osg::Group> inNode);
    /**
     * @brief Updates the View according to the Numbers stored in the Model
     */
    void update();
    
    /**
     * @brief Goes through the Model and adds a Geode for each Vertice stored in the Model to a Node in OSG
     *
     * @param inNode Node to which the vertice visualisation should be appended to
     */
    void visualizeVectors(MyVerticeVisualisation verticeVisualisation = Sphere);
    
    osg::ref_ptr<osg::Group> getRootNode();
    /**
     * @brief Removes all interaction mesh geodes from the _rootNode
     */
    void removeGeodes();
    /**
     * @brief Removes all vertice visualisation geodes from the _rootNode
     */
    void removeVisualisationGeodes();
    
private:
    /**
     * @brief Virtual function where the childclass creates a Geode from a Shape
     * 
     * @param shape the Shape which should be visualised by the Geode
     * @param color which color should the Geode get
     * 
     * @return Pointer to a Geode, which is a visualisation of the Shape
     */
    virtual osg::Geode* createGeodeFacade(Shape* shape, osg::Vec4 color) = 0;
    /**
     * @brief Virtual function where the childclass calculates a color for a Shape, based on the parameters
     * @param currentShape the shape for which the color should be calculated
     * @param oldShape the old state of the shape
     * @param oldColor the old color of the shape (actually the color of oldShape)
     * @return an osg 4 dimensional vector which describes the new color
     */
    virtual osg::Vec4f getColorFacade(Shape* currentShape, Shape* oldShape, osg::Vec4f oldColor = osg::Vec4f(0.0f, 0.0f, 0.0f, 0.0f)) = 0;
    /**
     * @brief Virtual function where the childclass creates a Geometry from a Shape
     * 
     * @param shape the Shape which should be visualised by the Geometry
     * @param color which color should the Geometry get
     * 
     * @return Pointer to a Geometry, which is a visualisation of the Shape
     */
    virtual osg::Geometry* createGeometryFacade(Shape* shape, osg::Vec4 color) = 0;
    
    /**
     * @brief Virtual function where the childclass constructs a Shape, based on an osg::Geometry and some Vertices
     * @details Actually converts the osg::Geometry back to Shape
     * @param geometry where the vertices of the Shape come from
     * @param verticeVec a Vertice Vector in which the Vertices that came from geometry are pushed
     * 
     * @return Shape which has to be deleted by the caller after using it!
     */
    virtual Shape* getShapeFromOSGGeometry(osg::Geometry* geometry, std::vector<Vertice>* verticeVec) = 0;
    
    /**
     * @brief Constructs a axe system
     * @param inVertice the vertice where to create the axe system
     * @paramn size size of a axis
     * @return The constructed axis as osg::Geode*
     */
    osg::Geode* createAxis(osg::Vec3 inVertice, double size);
    /**
     * @brief Constructs a Sphere
     * @param inVertice the vertice where to create the sphere
     * @paramn size size of the sphere
     * @return The constructed Sphere as osg::Geode*
     */
    osg::Geode* createSphere(osg::Vec3 inVertice, double size);
};

}
#endif /* defined(__BoneAnimation__MyInteractionMeshView__) */
