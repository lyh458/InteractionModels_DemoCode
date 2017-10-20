#ifndef LIGHTSOURCE_FINDER_H
#define LIGHTSOURCE_FINDER_H

#include <osg/NodeVisitor>
#include <osg/Node>
#include <osg/LightSource>


using namespace std;

class LightSourceFinder : public osg::NodeVisitor {
public:
	
	// Constructor - sets the traversal mode to TRAVERSE_ALL_CHILDREN
	// and Visitor type to NODE_VISITOR
	LightSourceFinder();
	
	// The 'apply' method for 'node' type instances.
	// See if a className() call to searchNode returns "Bone."
	// If so, add a pair <nodename,node> to our map.
	void apply(osg::Node &searchNode);
			
	// return a the list of nodes we found
	std::vector<osg::LightSource*> getNodeList();

	// utility function: 
	// remove light sources from a subgraph
	//
	// usage: 
	//         LightSourceFinder lsf;
	//         lsf.removeLightSources(loadedmodel);	
	void removeLightSources(osg::Node*);
	

private:

	std::vector<osg::LightSource*> _foundNodeList;

};

#endif
