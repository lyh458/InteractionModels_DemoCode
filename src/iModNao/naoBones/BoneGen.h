/*
 * CylinderCreator.h
 *
 *  Created on: Dec 17, 2013
 *      Author: Mengzhi Zhang, Hongchang Hao
 */

#ifndef BONEGEN_H
#define BONEGEN_H

#include "LineCreator.h"

class BoneGen
{
    public:

        BoneGen();
        virtual ~BoneGen();

        //generate bonepair map with the form (bone* child, bone* parent)
        //prepare for lines or cylinders create between child bone and parent bone
        map<osgAnimation::Bone*, osgAnimation::Bone*> genBonesFromBonemap(map<string,osgAnimation::Bone*> bonemap, osg::Node* vhuman, osg::Group* root, LineCreator* lc);
    protected:
    private:
};

#endif // BONEGEN_H
