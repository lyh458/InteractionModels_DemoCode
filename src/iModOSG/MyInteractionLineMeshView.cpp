//
//  MyInteractionLineMeshView.cpp
//  BoneAnimation
//
//  Created by Christian on 30.11.13.
//
//

#include "MyInteractionLineMeshView.h"

using namespace MyInteractionMesh;

MyInteractionLineMeshView::MyInteractionLineMeshView(MyInteractionMeshModel* model) {
    _model = model;
    _useSolidColor = false;
}

MyInteractionLineMeshView::~MyInteractionLineMeshView() {
    _model = NULL;
}

void MyInteractionLineMeshView::switchLineColoringToSolid(bool solidColor) {
    _useSolidColor = solidColor;
}

osg::Geode* MyInteractionLineMeshView::createGeodeFacade(Shape* shape, osg::Vec4 color) {
    Line* line = (Line*) shape;
    return createLineGeode(line->a->asOsgVec(), line->b->asOsgVec(), color);
}
osg::Vec4f MyInteractionLineMeshView::getColorFacade(Shape* currentShape, Shape* oldShape, osg::Vec4f oldColor) {
    if (_useSolidColor) {
        return osg::Vec4f(1.0f, 0.0f, 0.0f, 1.0f);
    } else {
        Line* currLine = (Line*) currentShape;
        Line* oldLine = (Line*) oldShape;
        return getColorForLine(std::make_pair(currLine->a->asOsgVec(), currLine->b->asOsgVec()),
                               std::make_pair(oldLine->a->asOsgVec(), oldLine->b->asOsgVec()),
                               oldColor);
    }
}
osg::Geometry* MyInteractionLineMeshView::createGeometryFacade(Shape* shape, osg::Vec4 color) {
    Line* line = (Line*) shape;
    return createLineGeometry(line->a->asOsgVec(), line->b->asOsgVec(), color);
}
Shape* MyInteractionLineMeshView::getShapeFromOSGGeometry(osg::Geometry* geometry, std::vector<Vertice>* verticeVec) {
    osg::Array *vecArray = geometry->getVertexArray();
    osg::Vec3Array *vec3Array = dynamic_cast<osg::Vec3Array*>(vecArray);
    osg::Vec3 oldVec1 = (*vec3Array)[0];
    osg::Vec3 oldVec2 = (*vec3Array)[1];
    verticeVec->push_back(Vertice(oldVec1));
    verticeVec->push_back(Vertice(oldVec2));
    Line *t = new Line(&(*verticeVec)[0], &(*verticeVec)[1]);
    return (Shape*) t;
    // currently you have to delete shape by yourself
}


osg::Geometry* MyInteractionLineMeshView::createLineGeometry(osg::Vec3 v1, osg::Vec3 v2, osg::Vec4 color) {
    osg::Geometry*  geometry = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array();
    
    vertices->push_back(v1);
    vertices->push_back(v2);
    vertices->dirty();
    geometry->setVertexArray(vertices);
    
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
    
/////OLD/COLOR -> WITHOUT BLENDING/LIGHTNING
//    osg::Vec4Array* colors   = new osg::Vec4Array();
//    colors->push_back(color);
//    colors->dirty();
//	
//#ifdef OSG_VERSION_LESS_THAN_3_DOT_2
//	geometry->setColorArray(colors); // bis OSG 3.1
//	geometry->setColorBinding(osg::Array::BIND_PER_PRIMITIVE_SET); // bis OSG 3.1
//#else
//	geometry->setColorArray(colors, osg::Array::BIND_PER_PRIMITIVE_SET); // erst ab OSG 3.2
//#endif
//	
//    geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
///////////
	
    
/////NEW/COLOR + GL_LIGHTNING/BLENDING
    // setting up lightning for triangle
    osg::StateSet* state = geometry->getOrCreateStateSet();
    osg::ref_ptr<osg::Material> material = new osg::Material;
    
    // Set alpha
    material->setAlpha(osg::Material::FRONT_AND_BACK, 1.0);
    material->setEmission(osg::Material::FRONT_AND_BACK, color);
    
    state->setAttributeAndModes( material.get() , osg::StateAttribute::ON |
                                osg::StateAttribute::MATERIAL);
    
    // Turn on blending
    osg::BlendFunc* blend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,
                                               osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
    state->setAttributeAndModes(blend);
    state->setMode(GL_BLEND, osg::StateAttribute::ON );
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON);
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
////////////
    
	osg::LineWidth* lineWidth = new osg::LineWidth();
	lineWidth->setWidth(4.0f);
	geometry->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
	
    
    geometry->dirtyDisplayList();
    geometry->setUseDisplayList(false);
    geometry->setDataVariance(osg::Object::DYNAMIC);
    
    return geometry;
}

osg::Geode* MyInteractionLineMeshView::createLineGeode(osg::Vec3 v1, osg::Vec3 v2, osg::Vec4 color) {
    osg::Geode*     geode    = new osg::Geode();
    osg::Geometry *geometry = createLineGeometry(v1, v2, color);
    geode->addDrawable(geometry);
    geode->setName("MyInteractionMesh_Line");
    return geode;
}

osg::Vec4f MyInteractionLineMeshView::getColorForLine(LineCoordinates current, LineCoordinates old, osg::Vec4f oldColor) {
    osg::Vec3 pq = current.second-current.first;
    osg::Vec4f color;
    float distance = pq.length();
    double r = 0;
    double g = 0;
    
    if (current.first == old.first && current.second == old.second) { // no old vertices given or no changes
        color = osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
    } else {
        osg::Vec3 p2q2 = old.second-old.first;
        float oldDistance = p2q2.length();
        r = oldColor.r();
        g = oldColor.g();
        
        if (distance < oldDistance) {
            if(r>=0.05) r = r-0.05;
            if(g<=0.95) g = g+0.05;
        } else {
            if(r<=0.95) r = r+0.05;
            if(g>=0.05) g = g-0.05;
        }
        color = osg::Vec4(r,g,0,1);
    }
    return color;
}
