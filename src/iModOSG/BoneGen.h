/*
 * BoneGen.h
 *
 *  Created on: Dec. 17, 2013
 *      Author: Mengzhi Zhang, Hongchang Hao
 */

#ifndef BONEGEN_H
#define BONEGEN_H

#include "LineCreator.h"
#include "CylinderCreator.h"

class BoneGen
{
    public:

        BoneGen();
        virtual ~BoneGen();

		/**
		 * generate bonepair map with the form (bone* child, bone* parent)
		 * prepare for lines or cylinders create between child bone and parent bone
		 * @param bonemap all bones and their names
		 * @param vhuman
		 * @param root
		 * @param lc  LineCreator
		 * @return bone pairs, which bones should be connected, the structur is <child, parents>
		 */
        map<osgAnimation::Bone*, osgAnimation::Bone*> genBonesFromBonemap(map<string,osgAnimation::Bone*> bonemap, osg::Node* vhuman, osg::Group* root, LineCreator* lc);

		//create tube skeleton
		void init(map<osgAnimation::Bone*, osgAnimation::Bone*> bp, CylinderCreator* cc, osg::ref_ptr<osg::Group> root, float radius = 0.02, osg::Vec4 color = osg::Vec4(0.34f, 0.38f, 0.4f, 1.0f));


    protected:
    private:
};

#endif // BONEGEN_H
