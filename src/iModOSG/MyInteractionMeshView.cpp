//
//  MyInteractionMeshView.cpp
//  BoneAnimation
//
//  Created by Christian on 25.11.13.
//
//

#include <iostream>
#include "MyInteractionMeshView.h"

#include "MyInteractionTriangleMesh.h"

using namespace MyInteractionMesh;

MyInteractionMeshView::MyInteractionMeshView() {
}

MyInteractionMeshView::~MyInteractionMeshView() {
    removeGeodes();
    removeVisualisationGeodes();
}

void MyInteractionMeshView::removeGeodes() {
    if (_rootNode) {
        int max = _geodeVector.size();
        for (int i=0; i<max; i++) {
            _rootNode->removeChild(_geodeVector.at(i));
        }
        _geodeVector.clear();
    }
}

osg::ref_ptr<osg::Group> MyInteractionMeshView::getRootNode() {
    return _rootNode;
}

void MyInteractionMeshView::update() {

    //std::cout<<"UpdateInteractionMeshView"<<std::endl;

    int max = _model->getConnectionVectorSize();
    for (int i = 0; i < max; ++i) {
        osg::Geode* geode = _geodeVector.at(i);
        
		if(!geode) {
			//std::cout<<"Geode fehlt!"<<std::endl;
			continue;
		}
		
		
		Shape* newShape = _model->getShape(i);
        
        osg::Geometry *geometry = (osg::Geometry*) geode->getDrawable(0);
    
        if(!geometry) {
			//std::cout<<"Geometry fehlt!"<<std::endl;
			continue;
		}
		
        Shape* oldShape = nullptr;
        std::vector<Vertice> verticeVec;
        
        oldShape = getShapeFromOSGGeometry(geometry,&verticeVec);
        // delete oldShape later!
        
        // if something has changed between oldShape and newShape
        if ( !oldShape->compareTo(newShape) ) {
            osg::Vec4 oldColor;
            osg::Material* oldMaterial = (osg::Material*) geometry->getOrCreateStateSet()->getAttribute(osg::StateAttribute::MATERIAL);
            
            // its possible that oldMaterial is NULL because after initially creating all shapes, some shapes could have
            // no valid face (like triangle(100,-75,100) and so OSG sets no material for this Geometry
            if(oldMaterial == nullptr) {
                oldColor = osg::Vec4f(1.0f, 1.0f, 0.0f, 0.0f);
            } else {
                oldColor = oldMaterial->getEmission(osg::Material::FRONT_AND_BACK);
            }
            
            
            osg::Vec4 newColor = getColorFacade(newShape, oldShape, oldColor);
            osg::Drawable* newDrawable = (osg::Drawable*) this->createGeometryFacade(newShape, newColor);
            
            geode->removeDrawables(0);
            geode->addDrawable(newDrawable);
        }
        delete oldShape;
    }
}

void MyInteractionMeshView::compileMeshAndAddToSceneGraph(osg::ref_ptr<osg::Group> inNode) {
    int max = _model->getConnectionVectorSize();
    _geodeVector.clear();
    for (int i = 0; i < max; ++i) {
        Shape* t = _model->getShape(i);
        
        osg::Vec4 color = getColorFacade(t, t);
        
        osg::Geode* g = this->createGeodeFacade(t, color);
        
        _geodeVector.push_back(g);
        inNode->addChild(g);
    }
    _rootNode = inNode;
}

void MyInteractionMeshView::visualizeVectors(MyVerticeVisualisation verticeVisualisation) {
    removeVisualisationGeodes();
    
    for (int v = toVH1; v <= toOther; v++) {
        MyInteractionMeshModelAddOption eCurrent = (MyInteractionMeshModelAddOption) v;
        int max = _model->getVerticeVectorSize(eCurrent);
        for (int i = 0; i < max; ++i) {
            Vertice* v = _model->getVertice(i, eCurrent);
            
            osg::Geode* geode = NULL;
            switch (verticeVisualisation) {
                case Axis:
                    geode = createAxis(v->asOsgVec(), 0.2);
                    break;
                case Sphere:
                    geode = createSphere(v->asOsgVec(), 0.05);
                    break;
                default:
                    return;
                    break;
            }
            //osg::Vec4 color = getColorFacade(t, t);
//            osg::Geode* g = this->createGeodeFacade(t, color);
            
            _verticeVizualitionVector.push_back(geode);
            _rootNode->addChild(geode);
        }
    }
}

void MyInteractionMeshView::removeVisualisationGeodes() {
    GeodeVector::iterator i;
    for (i = _verticeVizualitionVector.begin(); i != _verticeVizualitionVector.end(); ++i) {
        _rootNode->removeChild(*i.base());
    }
    
    _verticeVizualitionVector.clear();
}

osg::Geode* MyInteractionMeshView::createAxis(osg::Vec3 inVertice, double size) {
    osg::Geode*     geode    = new osg::Geode();
    osg::Geometry*  geometry = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array();
    osg::Vec4Array* colors   = new osg::Vec4Array();
    
    vertices->push_back(osg::Vec3(inVertice.x(), inVertice.y(), inVertice.z()));
    vertices->push_back(osg::Vec3(inVertice.x() + size, inVertice.y(), inVertice.z()));
    vertices->push_back(osg::Vec3(inVertice.x(), inVertice.y(), inVertice.z()));
    vertices->push_back(osg::Vec3(inVertice.x(), inVertice.y() + size, inVertice.z()));
    vertices->push_back(osg::Vec3(inVertice.x(), inVertice.y(), inVertice.z()));
    vertices->push_back(osg::Vec3(inVertice.x(), inVertice.y(), inVertice.z() + size));
    
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
    
    geometry->setVertexArray(vertices);
    geometry->setColorArray(colors);
    geometry->setColorBinding(osg::Geometry::BIND_PER_VERTEX);
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));
    geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
    
    geode->addDrawable(geometry);
    
    return geode;
}

osg::Geode* MyInteractionMeshView::createSphere(osg::Vec3 inVertice, double size) {
    osg::Sphere* sp = new osg::Sphere( inVertice, size);
    osg::ShapeDrawable* shapeDrawable= new osg::ShapeDrawable(sp);
    osg::Geode* g = new osg::Geode();
    g->addDrawable(shapeDrawable);
    return g;
}
