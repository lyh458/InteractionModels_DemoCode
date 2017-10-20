#include "LightSourceFinder.h"

LightSourceFinder::LightSourceFinder ()
: NodeVisitor (osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

void LightSourceFinder::apply (osg::Node &searchNode) {
	if (! strcmp (searchNode.className(), "LightSource")) {
		osg::LightSource* ls = (osg::LightSource*) &searchNode;
		_foundNodeList.push_back(ls);
	}
	
	traverse (searchNode);
}


std::vector<osg::LightSource*> LightSourceFinder::getNodeList() {
	return _foundNodeList;
}

// utility function:
// removes all light sources from subgraph below the node
//
// usage: 
//         LightSourceFinder lsf;
//         lsf.removeLightSources(loadedmodel);

void LightSourceFinder::removeLightSources(osg::Node* node) {
	this->apply(*node);
	std::vector<osg::LightSource*> lightsources = this->getNodeList();
	for (int i = 0; i < lightsources.size(); i++) {
		osg::LightSource* lightsource = lightsources[i];
		osg::Node::ParentList lightsourceparents = lightsource->getParents();
		for (int k = 0; k < lightsourceparents.size(); k++) {
			lightsourceparents[k]->removeChild(lightsource);
		}
	}
}
