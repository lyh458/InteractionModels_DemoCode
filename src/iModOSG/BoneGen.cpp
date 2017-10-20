#include "BoneGen.h"

BoneGen::BoneGen()
{
    //ctor
}

BoneGen::~BoneGen()
{
    //dtor
}

map<osgAnimation::Bone*, osgAnimation::Bone*> BoneGen::genBonesFromBonemap(map<string,osgAnimation::Bone*> bonemap, osg::Node* vhuman, osg::Group* root, LineCreator* lc)
{
	map<osgAnimation::Bone*, osgAnimation::Bone*> bp;
    map<std::string, osgAnimation::Bone*>::const_iterator it;
    osgAnimation::Bone* neck;

    for(it = bonemap.begin(); it != bonemap.end(); ++it)
    {
        if ((*it).first == "neckYaw")
        neck = (*it).second;
    }

    for(it = bonemap.begin(); it != bonemap.end(); ++it)
    {
        osgAnimation::Bone* parent = (*it).second->getBoneParent();
        string parentname = "-";
		if (parent)
		{
        parentname = parent->getName();

        osgAnimation::Bone* c = (osgAnimation::Bone*) lc->findNamedNode((*it).first, vhuman);
        osgAnimation::Bone* p = (osgAnimation::Bone*) lc->findNamedNode(parentname, vhuman);
		bp.insert(std::pair<osgAnimation::Bone*, osgAnimation::Bone*>(c,p));
		}
    }
    return bp;
}

void BoneGen::init(map<osgAnimation::Bone*, osgAnimation::Bone*> bp, CylinderCreator* cc, osg::ref_ptr<osg::Group> root, float radius, osg::Vec4 color)
{
    map<osgAnimation::Bone*, osgAnimation::Bone*>::const_iterator it;
    for(it = bp.begin(); it != bp.end(); ++it)
    {
        if((*it).first->getBoneParent()->getBoneParent())
        {
			cc->updateCylinderBetweenBones((*it).first, (*it).second, radius, root, color);
        }
    }
}
