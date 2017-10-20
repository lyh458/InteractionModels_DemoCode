//
//  MyInteractionTetrahedronMesh.cpp
//  BoneAnimation
//
//  Created by Christian on 27.11.13.
//
//

#include "MyInteractionTetrahedronMesh.h"

using namespace MyInteractionMesh;

MyInteractionTetrahedronMesh::MyInteractionTetrahedronMesh() {
    _connectionVector = (ShapeVector*) new TetrahedronVector();
}
MyInteractionTetrahedronMesh::~MyInteractionTetrahedronMesh() {
    ShapeVector::iterator i;
    for (i = _connectionVector->begin(); i != _connectionVector->end(); ++i) {
        delete (Tetrahedron*) (i.base());
    }
    _connectionVector->clear();
    delete _connectionVector;
}

Shape* MyInteractionTetrahedronMesh::getShape(int n) {
    return (Shape* ) getTetrahedron(n);
}

Tetrahedron* MyInteractionTetrahedronMesh::getTetrahedron(int n) {
    return (Tetrahedron*) _connectionVector->at(n);
}

Tetrahedron* MyInteractionTetrahedronMesh::addVerticesAndMakeTetrahedron(BoneAndContext a, BoneAndContext b, BoneAndContext c, BoneAndContext d) {
    Vertice *v1 = addVertice(a.first, a.second);
    Vertice *v2 = addVertice(b.first, b.second);
    Vertice *v3 = addVertice(c.first, c.second);
    Vertice *v4 = addVertice(d.first, d.second);
    
    return this->addTetrahedron(Tetrahedron(v1, v2, v3, v4));
}

Tetrahedron* MyInteractionTetrahedronMesh::addTetrahedron(Tetrahedron tetrahedron) {
    return addTetrahedron(tetrahedron.a, tetrahedron.b, tetrahedron.c, tetrahedron.d);
}

Tetrahedron* MyInteractionTetrahedronMesh::addTetrahedron(Vertice* a, Vertice* b, Vertice* c, Vertice* d) {
    Tetrahedron *tetra = new Tetrahedron(a,b,c,d);
    _connectionVector->push_back((Shape*) tetra);
    return tetra;
}
