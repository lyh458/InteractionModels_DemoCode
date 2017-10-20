//
//  MyInteractionLineMesh.cpp
//  BoneAnimation
//
//  Created by Christian on 27.11.13.
//
//

#include "MyInteractionLineMesh.h"

using namespace MyInteractionMesh;

MyInteractionLineMesh::MyInteractionLineMesh() {
    _connectionVector = (ShapeVector*) new LineVector();
}

MyInteractionLineMesh::~MyInteractionLineMesh() {
    ShapeVector::iterator i;
    for (i = _connectionVector->begin(); i != _connectionVector->end(); ++i) {
        delete (Line*) (i.base());
    }
    _connectionVector->clear();
    delete _connectionVector;
}

Line* MyInteractionLineMesh::addLine(Line line) {
    return addLine(line.a, line.b);
}

Line* MyInteractionLineMesh::addLine(Vertice* a, Vertice* b) {
    Line *line = new Line(a,b);
    _connectionVector->push_back((Shape*) line);
    return line;
}

Line* MyInteractionLineMesh::getLine(int n) {
    return (Line*) _connectionVector->at(n);
}

Shape* MyInteractionLineMesh::getShape(int n) {
    return (Shape* ) getLine(n);
}

Line* MyInteractionLineMesh::addVerticesAndMakeLine(BoneAndContext a, BoneAndContext b) {
    Vertice *v1 = addVertice(a.first, a.second);
    Vertice *v2 = addVertice(b.first, b.second);
    
    return this->addLine(Line(v1, v2));
}
