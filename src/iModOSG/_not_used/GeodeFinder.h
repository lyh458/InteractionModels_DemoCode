#ifndef GEODE_FINDER_H
#define GEODE_FINDER_H

#include <osg/NodeVisitor>
#include <osg/Node>
#include <osg/Geode>

using namespace std;

class GeodeFinder : public osg::NodeVisitor {
   public:

   // Constructor - sets the traversal mode to TRAVERSE_ALL_CHILDREN
   // and Visitor type to NODE_VISITOR
   GeodeFinder();

   // The 'apply' method for 'node' type instances.
   // See if a className() call to searchNode returns "Geode."
   // If so, add this node to our list.
   void apply(osg::Node &searchNode);

   // return a the list of nodes we found
   std::vector<osg::Geode*> getNodeList();

   // utility function:
   // set something for all geodes of a subgraph
   // e.g.: culling off or set NodeMask for all geode children of a loaded model

   void setGeodeDefaultSettings(osg::Node *node);

   private:
   // List of nodes with names that match the searchForName string
   std::vector<osg::Geode*> foundGeodes;
};

#endif
