//
//  MyInteractionLineMesh.h
//  BoneAnimation
//
//  Created by Christian on 27.11.13.
//
//

#ifndef __BoneAnimation__MyInteractionLineMesh__
#define __BoneAnimation__MyInteractionLineMesh__

#include "MyInteractionMeshModel.h"
#include <vector>

namespace MyInteractionMesh {
    

typedef std::pair<osg::Vec3, osg::Vec3> LineCoordinates;

/**
 * @brief Describes a Line
 * @details A Line is defined by 2 Vertices
 * 
 * @param A Point of Line which is described as Vertice
 * @param B Point of Line which is described as Vertice
 */
class Line : Shape {
public:
    Vertice* a;
    Vertice* b;
    Line(Vertice* A, Vertice* B) : a(A), b(B){}
    /**
     * @brief Gets the coordinates of the Line 
     * @return a std::pair of osg-Vectors
     */
    LineCoordinates getCoordinates() { return std::make_pair(a->asOsgVec(), b->asOsgVec()); }
    /**
     * @brief Calculates the lentgth of the Line
     * @return float value of the length
     */
    float getLength() {
        return (b->asOsgVec()-a->asOsgVec()).length();
    }
    /**
     * @brief Returns the length of the Line
     * @details Returns the length of the Line
     * 
     * @param a Vertice which describes the Line
     * @param b Vertice which describes the Line
     * 
     * @return length of the line as float value
     */
    static float getLength(Vertice a, Vertice b) {
        return (b.asOsgVec()-a.asOsgVec()).length();
    }

    bool compareTo(Shape* s) {
        Line* l = (Line*) s;
        return operator==(*l);
    }
    /**
     * @brief Equal-Operator
     * @param t compare to this Line
     * @return true if equal, false if not
     */
    bool operator==(const Line &t) {
        bool aa = t.a->operator==(*a);
        bool bb = t.b->operator==(*b);
        return (aa && bb);
    }
    /**
     * @brief Unequal-Operator 
     * @param t compare to this Line
     * @return true if unequal, false if not
     */
    bool operator!=(const Line &t) {
        return !operator==(t);
    }
    /**
     * @brief Deconstructor
     */
    virtual ~Line(){}
    int getNumberVertices() { return 2; }
};

typedef std::vector<Line> LineVector;

/**
 * @brief Model for a Line Mesh
 * @details Holds the model for a InteractionMesh made out of Line's
 */
class MyInteractionLineMesh : MyInteractionMeshModel {
    
public:
    /**
     * @brief Constructor
     */
    MyInteractionLineMesh();
    /**
     * @brief Deconstructor
     */
    virtual ~MyInteractionLineMesh();
    
    /**
     * @brief returns a specific item from the model (Getter)
     * 
     * @param n index of the item which should be returned
     * @return Pointer to a Shape
     */
    Shape* getShape(int n);
    /**
     * @brief Adds the Bones and their Vertice to the model and creates a Line
     * @details Adds the Bones and their Vertice and their information about the context (to which virtual human or object it belongs) to the model and creates a Line
     * 
     * @param a A Bone and his context (to which virtual human or object he belongs)
     * @param b A Bone and his context (to which virtual human or object he belongs)
     * 
     * @return Pointer to a Line which is created
     */
    Line* addVerticesAndMakeLine(BoneAndContext a, BoneAndContext b);
    
private:
    /**
     * @brief Gets a specific Line from the Model
     * 
     * @param n index of the Line
     * @return Pointer to the added Line
     */
    Line* getLine(int n);
    /**
     * @brief Adds a Line to the Model
     * 
     * @param line Line which should be added
     * @return Pointer to the added Line
     */
    Line* addLine(Line line);
    /**
     * @brief Adds a Line to the Model
     * 
     * @param a Start Vertice of the Line
     * @param b End Vertice of the Line
     * 
     * @return Pointer to the added Line
     */
    Line* addLine(Vertice* a, Vertice* b);
};

}
#endif /* defined(__BoneAnimation__MyInteractionLineMesh__) */
