#ifndef BONE_FINDER_H
#define BONE_FINDER_H

#include <osg/NodeVisitor>
#include <osg/Node>
#include <osgAnimation/Bone>

#include <map>

using namespace std;

class BoneFinder : public osg::NodeVisitor {
public:
	
	// Constructor - sets the traversal mode to TRAVERSE_ALL_CHILDREN
	// and Visitor type to NODE_VISITOR
	BoneFinder();
	
	// The 'apply' method for 'node' type instances.
	// See if a className() call to searchNode returns "Bone."
	// If so, add a pair <nodename,node> to our map.
	void apply(osg::Node &searchNode);
		
	// return a the map of nodes we found
	std::map<std::string,osgAnimation::Bone*> getNodeMap();
	
	// return a the list of nodes we found
	std::vector<osgAnimation::Bone*> getNodeList();


private:
	// List of nodes with names that match the searchForName string
	std::map<std::string,osgAnimation::Bone*> _foundBones;

	std::vector<osgAnimation::Bone*> _foundBonesList;

};

#endif
