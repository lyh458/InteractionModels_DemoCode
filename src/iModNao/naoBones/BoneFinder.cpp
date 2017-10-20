#include "BoneFinder.h"

BoneFinder::BoneFinder ()
: NodeVisitor (osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

void BoneFinder::apply (osg::Node &searchNode) {
	if (! strcmp (searchNode.className(), "Bone")) {
		osgAnimation::Bone* bone = (osgAnimation::Bone*) &searchNode;
		_foundBones.insert( std::pair<std::string,osgAnimation::Bone*>(bone->getName(),bone) );
	}

	traverse (searchNode);
}


std::map<std::string,osgAnimation::Bone*> BoneFinder::getNodeMap() {
	return _foundBones;
}
