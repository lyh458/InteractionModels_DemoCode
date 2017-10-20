//
//  MyInteractionLineMeshView.h
//  BoneAnimation
//
//  Created by Christian on 30.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionLineMeshView__
#define __BoneAnimation__MyInteractionLineMeshView__

#include "MyInteractionMeshView.h"
#include "MyInteractionLineMesh.h"


namespace MyInteractionMesh {

/**
 * @brief View for a InteractionMesh made out of Line's
 */
class MyInteractionLineMeshView : MyInteractionMeshView {
    
public:
	/**
	 * @brief Constructor
	 * 
	 * @param model Pointer to a model from which the numbers for the view come from.
	 */
    MyInteractionLineMeshView(MyInteractionMeshModel* model);
    /**
     * @brief Deconstructor
     */
    virtual ~MyInteractionLineMeshView();
    
    /**
     * @brief toggles the the coloring of lines between solid color and a changing color mode
     * @param solidColor if true, lines are colored as a solid blue color, if false lines are colored based on changes between updates
     */
    void switchLineColoringToSolid(bool solidColor);
    
private:
    /**
     * @brief if true lines got a solid blue color, if false color is calculated based on changes between update
     */
    bool _useSolidColor;
    
    osg::Geode* createGeodeFacade(Shape* shape, osg::Vec4 color);
    osg::Vec4f getColorFacade(Shape* currentShape, Shape* oldShape, osg::Vec4f oldColor = osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
    osg::Geometry* createGeometryFacade(Shape* shape, osg::Vec4 color);
    Shape* getShapeFromOSGGeometry(osg::Geometry* geometry, std::vector<Vertice>* verticeVec);
    
    /**
     * @brief Creates a Geode from a Line - Shape
     * @param v1 start Vertice of the Line
     * @param v2 end Vertice of the Line
     * @param color Color for the Geode
     * @return Pointer to the created Geode
     */
    osg::Geode* createLineGeode(osg::Vec3 v1, osg::Vec3 v2, osg::Vec4 color);
    /**
     * @brief Creates a Geometry from a Line - Shape
     * @param v1 start Vertice of the Line
     * @param v2 end Vertice of the Line
     * @param color Color for the Geometry
     * @return Pointer to the created Geometry
     */
    osg::Geometry* createLineGeometry(osg::Vec3 v1, osg::Vec3 v2, osg::Vec4 color);
    /**
     * @brief Calculates a color for a Line, based on the parameters
     * @param current the coordinates of the current Line for which the color should be calculated
     * @param old the old coordinates of the Line
     * @param oldColor the old color of the Line (actually the color for the old coordinates)
     * @return an osg 4 dimensional vector which describes the new color
     */
    osg::Vec4f getColorForLine(LineCoordinates current, LineCoordinates old, osg::Vec4f oldColor = osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f));
};
    
}

#endif /* defined(__BoneAnimation__MyInteractionLineMeshView__) */
