//
//  MyInteractionTetrahedronMesh.h
//  BoneAnimation
//
//  Created by Christian on 27.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionTetrahedronMesh__
#define __BoneAnimation__MyInteractionTetrahedronMesh__

#include "MyInteractionMeshModel.h"
#include <vector>
#include <osg/Matrix>

namespace MyInteractionMesh {
    
typedef boost::tuple<osg::Vec3, osg::Vec3, osg::Vec3, osg::Vec3> TetrahedronCoordinates;

/**
 * @brief Describes a Tetrahedron 
 * @param A Vertice 1 of a Tetrahedron
 * @param B Vertice 2 of a Tetrahedron
 * @param C Vertice 3 of a Tetrahedron
 * @param D Vertice 4 of a Tetrahedron
 */
class Tetrahedron : Shape {
public:
    Vertice* a; Vertice* b; Vertice* c; Vertice* d;
    /**
     * @brief Constructor
     * @param A Vertice 1 of a Tetrahedron
     * @param B Vertice 2 of a Tetrahedron
     * @param C Vertice 3 of a Tetrahedron
     * @param D Vertice 4 of a Tetrahedron
     */
    Tetrahedron(Vertice* A, Vertice* B, Vertice* C, Vertice* D) : a(A), b(B), c(C), d(D){}
    /**
     * @brief Deconstructor
     */
    virtual ~Tetrahedron(){}
    int getNumberVertices() { return 4; }
    
    
    /**
     * @brief Gets the coordinates of the Tetrahedron 
     * @return a 4-tuple (boost) of osg-Vectors
     */
    TetrahedronCoordinates getCoordinates() {
        return boost::make_tuple(a->asOsgVec(), b->asOsgVec(), c->asOsgVec(), d->asOsgVec());
    }
    /**
     * @brief Calculates the Volume of the Tetrahedron
     * @details actually uses the Volume-calculation described here: http://www.had2know.com/academics/tetrahedron-volume-4-vertices.html
     * @return double Value of the calculated Volume
     */
    double getVolume() {
        // http://www.had2know.com/academics/tetrahedron-volume-4-vertices.html
        double v11, v12, v13, v21, v22, v23, v31, v32, v33;
        v11 = a->x - d->x;
        v21 = a->y - d->y;
        v31 = a->z - d->z;
        
        v12 = b->x - d->x;
        v22 = b->y - d->y;
        v32 = b->z - d->z;
        
        v13 = c->x - d->x;
        v23 = c->y - d->y;
        v33 = c->z - d->z;

        double determinante = v11*v22*v33 + v12*v23*v31 + v13*v21*v32 - v13*v22*v31 - v12*v21*v33 - v11*v23*v32;
        double volume = determinante / 6;
        
        return volume;
        
    }
    /**
     * @brief Calculates the Volume of a Tetrahedron, defined by the parameters
     * @details actually uses the Volume-calculation described here: http://www.had2know.com/academics/tetrahedron-volume-4-vertices.html
     * @param a Vertice 1 of a Tetrahedron
     * @param b Vertice 2 of a Tetrahedron
     * @param c Vertice 3 of a Tetrahedron
     * @param d Vertice 4 of a Tetrahedron
     * @return double Value of the calculated Volume
     */
    static double getVolume(Vertice a, Vertice b, Vertice c, Vertice d) {
        double v11, v12, v13, v21, v22, v23, v31, v32, v33;
        v11 = a.x - d.x;
        v21 = a.y - d.y;
        v31 = a.z - d.z;
        
        v12 = b.x - d.x;
        v22 = b.y - d.y;
        v32 = b.z - d.z;
        
        v13 = c.x - d.x;
        v23 = c.y - d.y;
        v33 = c.z - d.z;
        
        double determinante = v11*v22*v33 + v12*v23*v31 + v13*v21*v32 - v13*v22*v31 - v12*v21*v33 - v11*v23*v32;
        double volume = determinante / 6;
        
        return volume;
    }
    bool compareTo(Shape* s){
        Tetrahedron* t = (Tetrahedron*) s;
        return  operator==(*t);
    }
    /**
     * @brief Equal-Operator
     * @param t compare to this Tetrahedron
     * @return true if equal, false if not
     */
    bool operator==(const Tetrahedron &t) {
        bool aa = t.a->operator==(*a);
        bool bb = t.b->operator==(*b);
        bool cc = t.c->operator==(*c);
        bool dd = t.d->operator==(*d);
        return (aa && bb && cc && dd);
    }
    /**
     * @brief Unequal-Operator 
     * @param t compare to this Tetrahedron
     * @return true if unequal, false if not
     */
    bool operator!=(const Tetrahedron &t) {
        return !operator==(t);
    }
};

typedef std::vector<Tetrahedron> TetrahedronVector;

/**
 * @brief Model for a InteractionMesh made out of Tetrahedron's
 * @details Holds the model for a InteractionMesh made out of Tetrahedron's
 */
class MyInteractionTetrahedronMesh : MyInteractionMeshModel {
    
public:
    /**
     * @brief Constructor
     */
    MyInteractionTetrahedronMesh();
    /**
     * @brief Desconstructor
     */
    virtual ~MyInteractionTetrahedronMesh();
    
    Shape* getShape(int n);

    /**
     * @brief Adds the Bones and their Vertice to the model and creates a Tetrahedron
     * @details Adds the Bones and their Vertice and their information about the context (to which virtual human or object it belongs) to the model and creates a Tetrahedron
     * 
     * @param a A Bone and his context (to which virtual human or object he belongs)
     * @param b A Bone and his context (to which virtual human or object he belongs)
     * @param c A Bone and his context (to which virtual human or object he belongs)
     * @param d A Bone and his context (to which virtual human or object he belongs)
     * 
     * @return Pointer to the Tetrahedron which is created
     */
    Tetrahedron* addVerticesAndMakeTetrahedron(BoneAndContext a, BoneAndContext b, BoneAndContext c, BoneAndContext d);
    
private:
    /**
     * @brief Gets a specific Tetrahedron from the Model
     * 
     * @param n integer index of the Tetrahedron
     * @return Pointer to the added Tetrahedron
     */
    Tetrahedron* getTetrahedron(int n);
    /**
     * @brief Adds a Tetrahedron to the Model
     * 
     * @param tetrahedron Tetrahedron which should be added
     * @return Pointer to the added Tetrahedron
     */
    Tetrahedron* addTetrahedron(Tetrahedron tetrahedron);
    /**
     * @brief Adds a Tetrahedron to the Model
     * 
     * @param a a Vertice of the Tetrahedron
     * @param b a Vertice of the Tetrahedron
     * @param c a Vertice of the Tetrahedron
     * @param d a Vertice of the Tetrahedron
     * 
     * @return Pointer to the added Tetrahedron
     */
    Tetrahedron* addTetrahedron(Vertice* a, Vertice* b, Vertice* c, Vertice* d);
};

}

#endif /* defined(__BoneAnimation__MyInteractionTetrahedronMesh__) */
