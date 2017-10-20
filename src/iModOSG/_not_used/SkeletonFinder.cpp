#include "SkeletonFinder.h"

SkeletonFinder::SkeletonFinder ()
: NodeVisitor (osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

void SkeletonFinder::apply (osg::Node &searchNode) {
	if (! strcmp (searchNode.className(), "Skeleton")) {
		_foundSkeletons.push_back ((osgAnimation::Skeleton*) &searchNode);
	}
	
	traverse (searchNode);
}

osgAnimation::Skeleton* SkeletonFinder::getFirst () {
	if (_foundSkeletons.size() > 0)
		return _foundSkeletons.at(0);
	else
		return NULL;
}

std::vector<osgAnimation::Skeleton*> SkeletonFinder::getNodeList() {
	return _foundSkeletons;
}


