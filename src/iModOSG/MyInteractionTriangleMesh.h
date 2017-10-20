//
//  MyInteractionTriangleMesh.h
//  BoneAnimation
//
//  Created by Christian on 27.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionTriangleMesh__
#define __BoneAnimation__MyInteractionTriangleMesh__

#include "MyInteractionMeshModel.h"
#include <vector>
#include <osgAnimation/Bone>
#include <boost/tuple/tuple.hpp>

namespace MyInteractionMesh {

typedef boost::tuple<osgAnimation::Bone*, osgAnimation::Bone*, osgAnimation::Bone*> BoneTuple;
typedef boost::tuple<osg::Vec3, osg::Vec3, osg::Vec3> TriangleCoordinates;

/**
 * @brief Describes a Triangle
 * @details A Triangle is defined by 3 Vertices
 * 
 * @param a Point of Triangle which is described as Vertice
 * @param b Point of Triangle which is described as Vertice
 * @param c Point of Triangle which is described as Vertice
 */
class Triangle : Shape {
public:
    Vertice* a; Vertice* b; Vertice* c;
    /**
     * @brief Constructor
     * @param A Vertice which describes the Triangle
     * @param B Vertice which describes the Triangle
     * @param C Vertice which describes the Triangle
     */
    Triangle(Vertice* A, Vertice* B, Vertice* C) : a(A), b(B), c(C){}
    /**
     * @brief Gets the coordinates of the Triangle 
     * @return a 3-tuple (boost) of osg-Vectors
     */
    TriangleCoordinates getCoordinates() { return boost::make_tuple(a->asOsgVec(), b->asOsgVec(), c->asOsgVec()); }
    /**
     * @brief Calculates the area of the Triangle
     * @return Area of the Triangle as float
     */
    float getArea() {
        osg::Vec3 ab = b->asOsgVec() - a->asOsgVec();
        osg::Vec3 ac = c->asOsgVec() - a->asOsgVec();
        return (ab^ac).length()/2;
    }
    /**
     * @brief Static function to calculate the area of a Triangle
     * 
     * @param a Vertice which describes the Triangle
     * @param b Vertice which describes the Triangle
     * @param c Vertice which describes the Triangle
     * @return Area of the Triangle as float
     */
    static float getArea(Vertice a, Vertice b, Vertice c) {
        osg::Vec3 ab = b.asOsgVec() - a.asOsgVec();
        osg::Vec3 ac = c.asOsgVec() - a.asOsgVec();
        return (ab^ac).length()/2;
    }
    bool compareTo(Shape* s){
        Triangle* t = (Triangle*) s;
        return  operator==(*t);
    }
    /**
     * @brief Equal-Operator
     * @param t compare to this Triangle
     * @return true if equal, false if not
     */
    bool operator==(const Triangle &t) {
        bool aa = t.a->operator==(*a);
        bool bb = t.b->operator==(*b);
        bool cc = t.c->operator==(*c);
        return (aa && bb && cc);
    }
    /**
     * @brief Unequal-Operator 
     * @param t compare to this Triangle
     * @return true if unequal, false if not
     */
    bool operator!=(const Triangle &t) {
        return !operator==(t);
    }
    /**
     * @brief Deconstructor
     */
    virtual ~Triangle(){}
    int getNumberVertices() { return 3; }
};
typedef std::vector<Triangle> TriangleVector;

/**
 * @brief Model for a Triangle Mesh
 * @details Holds the model for a InteractionMesh made out of Triangle's
 */
class MyInteractionTriangleMesh : MyInteractionMeshModel {
    
public:
    /**
     * @brief Constructor
     */
    MyInteractionTriangleMesh();
    /**
     * @brief Deconstructor
     */
    virtual ~MyInteractionTriangleMesh();

    Shape* getShape(int n);    
    /**
     * @brief Adds the Bones and their Vertice to the model and creates a Triangle
     * @details Adds the Bones and their Vertice and their information about the context (to which virtual human or object it belongs) to the model and creates a Triangle
     * 
     * @param a A Bone and his context (to which virtual human or object he belongs)
     * @param b A Bone and his context (to which virtual human or object he belongs)
     * @param c A Bone and his context (to which virtual human or object he belongs)
     * 
     * @return Pointer to a Triangle which is created
     */
    Triangle* addVerticesAndMakeTriangle(BoneAndContext a, BoneAndContext b, BoneAndContext c);
    
private:
    /**
     * @brief Gets a specific Triangle from the Model
     * 
     * @param n index of the Triangle
     * @return Pointer to the added Triangle
     */
    Triangle* getTriangle(int n);
    /**
     * @brief Adds a Triangle to the Model
     * 
     * @param triangle Triangle which should be added
     * @return Pointer to the added Triangle
     */
    Triangle* addTriangle(Triangle triangle);
    /**
     * @brief Adds a Triangle to the Model
     * 
     * @param a a Vertice of the Triangle
     * @param b a Vertice of the Triangle
     * @param c a Vertice of the Triangle     
     * 
     * @return Pointer to the added Triangle
     */
    Triangle* addTriangle(Vertice* a, Vertice* b, Vertice* c);

};

}
#endif /* defined(__BoneAnimation__MyInteractionTriangleMesh__) */
