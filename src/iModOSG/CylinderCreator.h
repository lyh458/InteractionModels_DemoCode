/*
 * CylinderCreator.h
 *
 *  Created on: Dec. 17, 2013
 *      Author: Mengzhi Zhang, Hongchang Hao
 */


#ifndef CYLINDERCREATOR_H
#define CYLINDERCREATOR_H


#include <osg/ShapeDrawable>
#include <osg/Shape>
#include <osgAnimation/UpdateBone>
#include "LineCreator.h"

class CylinderCreator : public LineCreator::LineCreator
{

        std::map<std::pair<osgAnimation::Bone*,osgAnimation::Bone*> , osg::Geode*> _container;
    public:
        CylinderCreator();
        virtual ~CylinderCreator();

		/**
		 * adds update a Cylinder between start bone and end bone in osg group inRootNode
		 * @param start start Bone
		 * @param end end Bone
		 * @param radius radius of cylinder
		 * @param inRootNode root for the Bones
		 */
		void updateCylinderBetweenBones(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius, osg::ref_ptr<osg::Group> inRootNode, osg::Vec4 color);



		/**
		 * creates a geode between 2 bones and adds it to the container
		 * @param start start Bone
		 * @param end end Bone
		 * @param radius radius of cylinder
		 */
        osg::Geode* addGeode(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius);

		/**
		 *  returns a the geode between 2 bones
		 * @param start start Bone
		 * @param end end Bone
		 */
        osg::Geode* getGeode(osgAnimation::Bone* start, osgAnimation::Bone* end);


		/**
		 * check if there is Geode between two bones
		 * @param start start Bone
		 * @param end end Bone
		 * @return bool
		 */
        bool isGeodeInContainerForCylinder(osgAnimation::Bone* start, osgAnimation::Bone* end);

		/**
		 * creates a geode( cylinder and two spheres ) which begins at start vertice and ends at end vertice
		 * @param start coordinate of the start point
		 * @param end coordinate of the end point
		 * @param radius radius of cylinder
		 * @param color white color
		 * @return a gernerated cylinder geode
		 */
		osg::Geode* createCylinderGeode(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color = osg::Vec4(0.34f, 0.38f, 0.4f, 1.0f));

		/**
		 * creates a cylinder geometry
		 * @param start coordinate of the start point
		 * @param end coordinate of the end point
		 * @param radius radius of cylinder
		 * @param color white color
		 * @return a gernerated cylinder geometry
		 */
		osg::Geometry* createGeometry(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color/* = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f)*/);

		/**
		 * help function get the stacked trans from a bone
		 * @param bone
		 * @return trans to its parent
		 */
		osg::Vec3 getBoneStackedTrans(osgAnimation::Bone* bone);


};

#endif // CYLINDERCREATOR_H
