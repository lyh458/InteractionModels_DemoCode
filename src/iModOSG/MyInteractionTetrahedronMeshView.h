//
//  MyInteractionTetrahedronMeshView.h
//  BoneAnimation
//
//  Created by Christian on 30.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionTetrahedronMeshView__
#define __BoneAnimation__MyInteractionTetrahedronMeshView__

#include "MyInteractionMeshView.h"
#include "MyInteractionTetrahedronMesh.h"

namespace MyInteractionMesh {
/**
 * @brief View for a InteractionMesh made out of Tetrahedron's
 */
class MyInteractionTetrahedronMeshView : MyInteractionMeshView {
    
public:
	/**
	 * @brief Constructor
	 * 
	 * @param model Pointer to a model from which the numbers for the view come from.
	 */
    MyInteractionTetrahedronMeshView(MyInteractionMeshModel* model);
     /**
     * @brief Deconstructor
     */
    virtual ~MyInteractionTetrahedronMeshView();
    
private:
    osg::Geode* createGeodeFacade(Shape* shape, osg::Vec4 color);
    osg::Vec4f getColorFacade(Shape* currentShape, Shape* oldShape, osg::Vec4f oldColor = osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
    osg::Geometry* createGeometryFacade(Shape* shape, osg::Vec4 color);
    Shape* getShapeFromOSGGeometry(osg::Geometry* geometry, std::vector<Vertice>* verticeVec);
    /**
     * @brief Creates a Geode from a Tetrahedron - Shape
     * @param v1 a Vertice of the Tetrahedron
     * @param v2 a Vertice of the Tetrahedron
     * @param v3 a Vertice of the Tetrahedron
     * @param v4 a Vertice of the Tetrahedron
     * @param color Color for the Geode
     * @return Pointer to the created Geode
     */
    osg::Geode* createTetrahedronGeode(osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec3 v4, osg::Vec4 color);
    /**
     * @brief Creates a Geometry from a Tetrahedron - Shape
     * @param v1 a Vertice of the Tetrahedron
     * @param v2 a Vertice of the Tetrahedron
     * @param v3 a Vertice of the Tetrahedron
     * @param v4 a Vertice of the Tetrahedron
     * @param color Color for the Geometry
     * @return Pointer to the created Geometry
     */
    osg::Geometry* createTetrahedronGeometry(osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec3 v4, osg::Vec4 color);
    /**
     * @brief Calculates a color for a Tetrahedron, based on the parameters
     * @param current the coordinates of the current Tetrahedron for which the color should be calculated
     * @param old the old coordinates of the Tetrahedron
     * @param oldColor the old color of the Tetrahedron (actually the color for the old coordinates)
     * @return an osg 4 dimensional vector which describes the new color
     */
    osg::Vec4f getColorForTetrahedron(TetrahedronCoordinates current, TetrahedronCoordinates old, osg::Vec4f oldColor = osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));    
};

}
#endif /* defined(__BoneAnimation__MyInteractionTetrahedronMeshView__) */
