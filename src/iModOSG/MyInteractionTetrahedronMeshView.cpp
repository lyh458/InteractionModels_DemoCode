//
//  MyInteractionTetrahedronMeshView.cpp
//  BoneAnimation
//
//  Created by Christian on 30.11.13.
//
//

#include "MyInteractionTetrahedronMeshView.h"

using namespace MyInteractionMesh;


MyInteractionTetrahedronMeshView::MyInteractionTetrahedronMeshView(MyInteractionMeshModel* model) {
    _model = model;
}

MyInteractionTetrahedronMeshView::~MyInteractionTetrahedronMeshView() {
    _model = nullptr;
}

osg::Geode* MyInteractionTetrahedronMeshView::createGeodeFacade(Shape* shape, osg::Vec4 color) {
    Tetrahedron* tetra = (Tetrahedron*) shape;
    return createTetrahedronGeode(tetra->a->asOsgVec(),
                                  tetra->b->asOsgVec(),
                                  tetra->c->asOsgVec(),
                                  tetra->d->asOsgVec(),
                                  color);
}
osg::Geometry* MyInteractionTetrahedronMeshView::createGeometryFacade(Shape* shape, osg::Vec4 color) {
    Tetrahedron* tetra = (Tetrahedron*) shape;
    return createTetrahedronGeometry(tetra->a->asOsgVec(),
                                     tetra->b->asOsgVec(),
                                     tetra->c->asOsgVec(),
                                     tetra->d->asOsgVec(),
                                     color);
}

osg::Vec4f MyInteractionTetrahedronMeshView::getColorFacade(Shape* currentShape, Shape* oldShape, osg::Vec4f oldColor) {
    Tetrahedron* currTetra= (Tetrahedron*) currentShape;
    Tetrahedron* oldTetra = (Tetrahedron*) oldShape;
    return getColorForTetrahedron(
                            boost::make_tuple(currTetra->a->asOsgVec(),
                                              currTetra->b->asOsgVec(),
                                              currTetra->c->asOsgVec(),
                                              currTetra->d->asOsgVec()),
                            boost::make_tuple(oldTetra->a->asOsgVec(),
                                              oldTetra->b->asOsgVec(),
                                              oldTetra->c->asOsgVec(),
                                              oldTetra->d->asOsgVec()),
                            oldColor);
}

Shape* MyInteractionTetrahedronMeshView::getShapeFromOSGGeometry(osg::Geometry* geometry, std::vector<Vertice>* verticeVec) {
    osg::Array *vecArray = geometry->getVertexArray();
    osg::Vec3Array *vec3Array = dynamic_cast<osg::Vec3Array*>(vecArray);
    osg::Vec3 oldVec1 = (*vec3Array)[0];
    osg::Vec3 oldVec2 = (*vec3Array)[1];
    osg::Vec3 oldVec3 = (*vec3Array)[2];
    osg::Vec3 oldVec4 = (*vec3Array)[3];
    
    verticeVec->push_back(Vertice(oldVec1));
    verticeVec->push_back(Vertice(oldVec2));
    verticeVec->push_back(Vertice(oldVec3));
    verticeVec->push_back(Vertice(oldVec4));
    
    Tetrahedron *t = new Tetrahedron(&(*verticeVec)[0], &(*verticeVec)[1], &(*verticeVec)[2], &(*verticeVec)[3]);
    return (Shape*) t;
    // currently have to delete shape by yourself
}


osg::Geode* MyInteractionTetrahedronMeshView::createTetrahedronGeode(osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec3 v4,osg::Vec4 color) {
    osg::Geode*     geode    = new osg::Geode();
    
    geode->addDrawable(this->createTetrahedronGeometry(v1, v2, v3, v4, color));
    geode->setName("MyInteractionMesh_Tetrahedron");
    return geode;
}

osg::Geometry* MyInteractionTetrahedronMeshView::createTetrahedronGeometry(osg::Vec3 v1, osg::Vec3 v2, osg::Vec3 v3, osg::Vec3 v4, osg::Vec4 color) {
    osg::Geometry*  geometry = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array();
    
    vertices->push_back(v1);
    vertices->push_back(v2);
    vertices->push_back(v3);
    vertices->push_back(v4);
    
    vertices->dirty();
    geometry->setVertexArray(vertices);
    
    osg::DrawElementsUInt* base = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    base->push_back( 2 );
    base->push_back( 1 );
    base->push_back( 0 );
    
    osg::DrawElementsUInt* face1 = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    face1->push_back( 3 );
    face1->push_back( 1 );
    face1->push_back( 0 );
    
    osg::DrawElementsUInt* face2 = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    face2->push_back( 3 );
    face2->push_back( 2 );
    face2->push_back( 1 );

    osg::DrawElementsUInt* face3 = new osg::DrawElementsUInt( osg::PrimitiveSet::TRIANGLES, 0 );
    face3->push_back( 3 );
    face3->push_back( 2 );
    face3->push_back( 0 );
    
    
    geometry->addPrimitiveSet( base );
    geometry->addPrimitiveSet( face1 );
    geometry->addPrimitiveSet( face2 );
    geometry->addPrimitiveSet( face3 );
    
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
    material->setAlpha(osg::Material::FRONT_AND_BACK, 0.3);
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
    
//	osg::LineWidth* lineWidth = new osg::LineWidth();
//	lineWidth->setWidth(1.0f);
//	state->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
	
    
    geometry->dirtyDisplayList();
    geometry->setUseDisplayList(false);
    geometry->setDataVariance(osg::Object::DYNAMIC);
    
    return geometry;

}

osg::Vec4f MyInteractionTetrahedronMeshView::getColorForTetrahedron(TetrahedronCoordinates current, TetrahedronCoordinates old, osg::Vec4f oldColor) {
    double areaNew = Tetrahedron::getVolume(Vertice(current.get<0>()),
                                           Vertice(current.get<1>()),
                                           Vertice(current.get<2>()),
                                           Vertice(current.get<3>()));
    double areaOld = Tetrahedron::getVolume(Vertice(old.get<0>()),
                                           Vertice(old.get<1>()),
                                           Vertice(old.get<2>()),
                                           Vertice(old.get<3>()));
    
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
