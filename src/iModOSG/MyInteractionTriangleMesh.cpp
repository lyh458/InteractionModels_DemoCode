//
//  MyInteractionTriangleMesh.cpp
//  BoneAnimation
//
//  Created by Christian on 27.11.13.
//
//

#include "MyInteractionTriangleMesh.h"

using namespace MyInteractionMesh;

MyInteractionTriangleMesh::MyInteractionTriangleMesh() {
    _connectionVector = (ShapeVector*) new TriangleVector();
}

MyInteractionTriangleMesh::~MyInteractionTriangleMesh() {
    ShapeVector::iterator i;
    for (i = _connectionVector->begin(); i != _connectionVector->end(); ++i) {
        delete (Triangle*) (i.base());
    }
    _connectionVector->clear();
    delete _connectionVector;
}


Triangle* MyInteractionTriangleMesh::addVerticesAndMakeTriangle(BoneAndContext a, BoneAndContext b, BoneAndContext c) {
    Vertice *v1 = addVertice(a.first, a.second);
    Vertice *v2 = addVertice(b.first, b.second);
    Vertice *v3 = addVertice(c.first, c.second);
    
    return this->addTriangle(Triangle(v1, v2, v3));
}

Triangle* MyInteractionTriangleMesh::addTriangle(Triangle triangle) {
    return addTriangle(triangle.a, triangle.b, triangle.c);
}

Triangle* MyInteractionTriangleMesh::addTriangle(Vertice* a, Vertice* b, Vertice* c) {
    Triangle *tri = new Triangle(a,b,c);
    _connectionVector->push_back((Shape*) tri);

    return tri;
}

Triangle* MyInteractionTriangleMesh::getTriangle(int n) {
    return (Triangle*) _connectionVector->at(n);
}

Shape* MyInteractionTriangleMesh::getShape(int n) {
    return (Shape* ) getTriangle(n);
}
