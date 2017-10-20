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
        if((c->getName() != "r_shoulder" )&&   (c->getName() != "l_shoulder" ))
                bp.insert(std::pair<osgAnimation::Bone*, osgAnimation::Bone*>(c,p));
        else
                bp.insert(std::pair<osgAnimation::Bone*, osgAnimation::Bone*>(c, neck));
		}
    }
    return bp;
}

