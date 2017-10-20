/*
 * CylinderCreator.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Mengzhi Zhang, Hongchang Hao
 */


#ifndef CYLINDERCREATOR_H
#define CYLINDERCREATOR_H


#include <osg/ShapeDrawable>
#include <osg/Shape>
#include "LineCreator.h"

class CylinderCreator : public LineCreator::LineCreator
{

        std::map<std::pair<osgAnimation::Bone*,osgAnimation::Bone*> , osg::Geode*> _container;
    public:
        CylinderCreator();
        virtual ~CylinderCreator();

        // adds / update a Cylinder between start bone and end bone in osg group inRootNode
        void updateCylinderBetweenBones(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius, osg::ref_ptr<osg::Group> inRootNode);

    protected:
    private:

        // creates a geode between 2 bones and adds it to the container
        osg::Geode* addGeode(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius);

        // returns a the geode between 2 bones
        osg::Geode* getGeode(osgAnimation::Bone* start, osgAnimation::Bone* end);

        bool isGeodeInContainerForCylinder(osgAnimation::Bone* start, osgAnimation::Bone* end);

        // creates a geode( cylinder and two spheres ) which begins at start vertice and ends at end vertice
        osg::Geode* createCylinderGeode(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));

        // creates a cylinder geometry
        osg::Geometry* createGeometry(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
};

#endif // CYLINDERCREATOR_H
