#ifndef SKELETON_FINDER_H
#define SKELETON_FINDER_H

#include <osg/NodeVisitor>
#include <osg/Node>
#include <osgAnimation/Skeleton>

using namespace std;

class SkeletonFinder : public osg::NodeVisitor {
public:
	
	// Constructor - sets the traversal mode to TRAVERSE_ALL_CHILDREN
	// and Visitor type to NODE_VISITOR
	SkeletonFinder();
	
	// The 'apply' method for 'node' type instances.
	// See if a className() call to searchNode returns "Skeleton."
	// If so, add this node to our list.
	void apply(osg::Node &searchNode);
	
	// Return a pointer to the first node in the list
	// with a matching name
	osgAnimation::Skeleton* getFirst();
	
	// return a the list of nodes we found
	std::vector<osgAnimation::Skeleton*> getNodeList();
	
private:
	// List of nodes with names that match the searchForName string
	std::vector<osgAnimation::Skeleton*> _foundSkeletons;
};

#endif