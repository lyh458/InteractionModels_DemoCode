//
//  MyInteractionMesh.h
//  BoneAnimation
//
//  Created by Christian on 27.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionMeshModel__
#define __BoneAnimation__MyInteractionMeshModel__

#include <vector>
#include <boost/tuple/tuple.hpp>
#include <osgAnimation/Bone>
#include <iostream>

namespace MyInteractionMesh {

enum MyInteractionMeshModelAddOption {
    toVH1 = 0,
    toVH2 = 1,
    toOther = 2
};

enum MyMeshKind {
    LINE = 0,           // has to be 0, add new to the bottom
    TRIANGLE = 1,       // has to be 1, add new to the bottom
    TETRAHEDRON = 2,     // has to be 2, add new to the bottom
    LINE_1COLOR = 3,     // has to be 3, add new to the bottom and before OFF
    OFF = 4,
};


/**
 * @brief Baseclass of all Shapes (Line, Triangle, Tetrahedron)
 */
class Shape {
public:
    /**
     * @brief Constructor
     */
    Shape() {}
    /**
     * @brief Deconstructor
     */
    virtual ~Shape() {}
    /**
     * @brief Gets the Number of Vertices in the Shape
     * @return integer number of vertices in the Shape
     */
    virtual int getNumberVertices() = 0;
    /**
     * @brief Compares the Shape to another
     * 
     * @param s the other shape to which it should be compared to
     * @return true if equal, false if not
     */
    virtual bool compareTo(Shape* s) = 0;
};

/**
 * @brief A Vertice in the 3 dimensional space
 * 
 * @param X coordinate in x axis
 * @param Y coordinate in y axis
 * @param Z coordinate in z axis
 */
class Vertice {
public:
    double x,y,z;
    /**
     * @brief Constructor
     * @param X coordinate in x axis
     * @param Y coordinate in y axis
     * @param Z coordinate in z axis
     */
    Vertice(double X, double Y, double Z) : x(X), y(Y), z(Z) {}
    /**
     * @brief Constructor
     * @param osgVec Vertice in 3 dimensional space as osg::Vec3 (Openscenegraph 3D-Vertice)
     */
    Vertice(osg::Vec3 osgVec) : x(osgVec.x()), y(osgVec.y()), z(osgVec.z()) {}
    /**
     * @brief Deconstructor
     */
    virtual ~Vertice() {}
    /**
     * @brief sets the class attributes
     * @param osgVec Vertice in 3 dimensional space as osg::Vec3 (Openscenegraph 3D-Vertice)
     */
    void setValues(osg::Vec3 osgVec) {
        x = osgVec.x();
        y = osgVec.y();
        z = osgVec.z();
    }
    bool operator==(const Vertice &v)const{
        return  (x == v.x &&
                y == v.y &&
                z == v.z);
    }
    bool operator!=(const Vertice &v)const{
        return !operator==(v);
    }
    
    osg::Vec3 asOsgVec() { return osg::Vec3(x,y,z); }
};

typedef std::pair<Vertice*, osgAnimation::Bone*> VerticeAndBonePtr;
typedef std::vector<VerticeAndBonePtr> VerticeVector;
typedef std::vector<Shape*> ShapeVector;
    
typedef std::pair<osgAnimation::Bone*, MyInteractionMeshModelAddOption> BoneAndContext;
typedef boost::tuple<BoneAndContext, BoneAndContext> BoneAndContext2Tuple;
typedef boost::tuple<BoneAndContext, BoneAndContext, BoneAndContext> BoneAndContext3Tuple;
typedef boost::tuple<BoneAndContext, BoneAndContext, BoneAndContext, BoneAndContext> BoneAndContext4Tuple;

/**
 * @brief Baseclass for all Models in the Model-View-Controller pattern of the MyInteractionMesh
 */
class MyInteractionMeshModel {
    VerticeVector _verticesVH1;
    VerticeVector _verticesVH2;
    VerticeVector _verticesOther;
    
public:
    /**
     * @brief Deconstructor
     */
    virtual ~MyInteractionMeshModel();
    /**
     * @brief Updates the Vertices in the Model, based on the current matrices in the Bones which they belong to
     * @param rootNode the rootNode in which the InteractionMesh is appended
     */
    void update(osg::ref_ptr<osg::Group> rootNode);
    /**
     * @brief Gets the number of Shapes in the model
     * @return [description]
     */
    int getConnectionVectorSize();
    /**
     * @brief Gets a specific Shape from the Model
     * 
     * @param n index number of the Shape
     * @return Pointer to a Shape
     */
    virtual Shape* getShape(int n) = 0;
    
    Vertice* getVertice(int n, MyInteractionMeshModelAddOption whichVector);
    
    int getVerticeVectorSize(MyInteractionMeshModelAddOption whichVector);
    
private:
    ShapeVector* getConnectionVector();
    
    Vertice* searchBoneInVector(osgAnimation::Bone* target, MyInteractionMeshModelAddOption inVH);
    
protected:
    /**
     * @brief Adds a Vertice to the Model
     * @details For update reasons its important to add the Vertice from an Bone and for semantic reasons the Virtual Human from which this Bone comes from
     * @param fromBone the Vertice is extracted from this Bone
     * @param toVH Context information from which Virtual Human or Object this Bone is from
     * 
     * @return Pointer to the Vertice that was added
     */
    Vertice* addVertice(osgAnimation::Bone* fromBone, MyInteractionMeshModelAddOption toVH);
    ShapeVector* _connectionVector;
};
}

#endif /* defined(__BoneAnimation__MyInteractionMesh__) */
