//
//  MyInteractionTriangleMeshView.cpp
//  BoneAnimation
//
//  Created by Christian on 30.11.13.
//
//

#include "MyInteractionTriangleMeshView.h"

using namespace MyInteractionMesh;

MyInteractionTriangleMeshView::MyInteractionTriangleMeshView(MyInteractionMeshModel* model) {
    _model = model;
}

MyInteractionTriangleMeshView::~MyInteractionTriangleMeshView() {
    _model = NULL;
}

osg::Geode* MyInteractionTriangleMeshView::createGeodeFacade(Shape* shape, osg::Vec4 color) {
    Triangle* triangle = (Triangle*) shape;
    return createTriangleGeode(triangle->a->asOsgVec(), triangle->b->asOsgVec(), triangle->c->asOsgVec(), color);
}
osg::Geometry* MyInteractionTriangleMeshView::createGeometryFacade(Shape* shape, osg::Vec4 color) {
    Triangle* triangle = (Triangle*) shape;
    return createTriangleGeometry(triangle->a->asOsgVec(), triangle->b->asOsgVec(), triangle->c->asOsgVec(), color);
}

osg::Vec4f MyInteractionTriangleMeshView::getColorFacade(Shape* currentShape, Shape* oldShape, osg::Vec4f oldColor) {
    Triangle* currTri = (Triangle*) currentShape;
    Triangle* oldTri = (Triangle*) oldShape;
    return getColorForTriangle(boost::make_tuple(currTri->a->asOsgVec(), currTri->b->asOsgVec(), currTri->c->asOsgVec()),
                               boost::make_tuple(oldTri->a->asOsgVec(), oldTri->b->asOsgVec(), oldTri->c->asOsgVec()),
                               oldColor);
}

Shape* MyInteractionTriangleMeshView::getShapeFromOSGGeometry(osg::Geometry* geometry, std::vector<Vertice>* verticeVec) {
    osg::Array *vecArray = geometry->getVertexArray();
    osg::Vec3Array *vec3Array = dynamic_cast<osg::Vec3Array*>(vecArray);
    osg::Vec3 oldVec1 = (*vec3Array)[0];
    osg::Vec3 oldVec2 = (*vec3Array)[1];
    osg::Vec3 oldVec3 = (*vec3Array)[2];
    
    verticeVec->push_back(Vertice(oldVec1));
    verticeVec->push_back(Vertice(oldVec2));
    verticeVec->push_back(Vertice(oldVec3));
    
    Triangle *t = new Triangle(&(*verticeVec)[0], &(*verticeVec)[1], &(*verticeVec)[2]);

    return (Shape*) t;
    
    // currently have to delete shape by yourself
}


osg::Geode* MyInteractionTriangleMeshView::createTriangleGeode(osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec4 color) {
    osg::Geode*     geode    = new osg::Geode();
    
    geode->addDrawable(this->createTriangleGeometry(v1, v2, v3, color));
    geode->setName("MyInteractionMesh_Triangle");
    return geode;
}

osg::Geometry* MyInteractionTriangleMeshView::createTriangleGeometry(osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec4 color) {
    osg::Geometry*  geometry = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array();
    //    osg::Vec4Array* colors   = new osg::Vec4Array();
    
    vertices->push_back(v1);
    vertices->push_back(v2);
    vertices->push_back(v3);
    vertices->dirty();
    
    geometry->setVertexArray(vertices);
    
    // create a primitive set (add index numbers for vertices of triangle)
    osg::DrawElementsUInt* primitiveSet = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    primitiveSet->push_back( 2 );
    primitiveSet->push_back( 1 );
    primitiveSet->push_back( 0 );
    geometry->addPrimitiveSet( primitiveSet );
    
    // setting up lightning for triangle
    osg::StateSet* state = geometry->getOrCreateStateSet();
    osg::ref_ptr<osg::Material> material = new osg::Material;
    
    // Set alpha
    material->setAlpha(osg::Material::FRONT_AND_BACK, 0.5);
    material->setEmission(osg::Material::FRONT_AND_BACK, color);
    
    state->setAttributeAndModes( material.get() , osg::StateAttribute::ON |
                                osg::StateAttribute::MATERIAL);
    
    // Turn on blending
    osg::BlendFunc* blend = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA,
                                               osg::BlendFunc::ONE_MINUS_SRC_ALPHA );
    state->setAttributeAndModes(blend);
    state->setMode(GL_BLEND, osg::StateAttribute::ON );
    state->setMode(GL_LIGHTING, osg::StateAttribute::ON );
    state->setRenderingHint( osg::StateSet::TRANSPARENT_BIN );
    
    
    // standardshading
    // geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	
    // create color array data (each corner of our triangle will have one color component)
    // wont work when using Materials when fiddling with shading/transparency
    //    colors->push_back( osg::Vec4( 1.0f, 0.0f, 0.0f, 1.0f ) );
    //    colors->push_back( osg::Vec4( 0.0f, 1.0f, 0.0f, 1.0f ) );
    //    colors->push_back( osg::Vec4( 0.0f, 0.0f, 1.0f, 1.0f ) );
    //    colors->dirty();
    //
    //#ifdef OSG_VERSION_LESS_THAN_3_DOT_2
    //	geometry->setColorArray(colors); // bis OSG 3.1
    //	geometry->setColorBinding (osg::Geometry::BIND_PER_VERTEX); // bis OSG 3.1
    //#else
    //	geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX); // erst ab OSG 3.2
    //#endif
    
    
    //	osg::LineWidth* lineWidth = new osg::LineWidth();
    //	lineWidth->setWidth(3.0f);
    //	geometry->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
	
    
    geometry->dirtyDisplayList();
    geometry->setUseDisplayList(false);
    geometry->setDataVariance(osg::Object::DYNAMIC);
    return geometry;
}

osg::Vec4f MyInteractionTriangleMeshView::getColorForTriangle(TriangleCoordinates current, TriangleCoordinates old, osg::Vec4f oldColor) {
    float areaNew = Triangle::getArea(Vertice(current.get<0>()), Vertice(current.get<1>()), Vertice(current.get<2>()));
    float areaOld = Triangle::getArea(Vertice(old.get<0>()), Vertice(old.get<1>()), Vertice(old.get<2>()));
    osg::Vec4f newColor;
    double r,g;
    r = oldColor.r();
    g = oldColor.g();
    
    if (areaNew < areaOld) {
        if(r>=0.05) r = r-0.05;
        if(g<=0.95) g = g+0.05;
        newColor = osg::Vec4f(r,g,0.0f, 1.0f);
    } else if (areaNew > areaOld) {
        if(r<=0.95) r = r+0.05;
        if(g>=0.05) g = g-0.05;
        newColor = osg::Vec4f(r,g,0.0f, 1.0f);
    } else {
        newColor = oldColor;
    }
    return newColor;
}
