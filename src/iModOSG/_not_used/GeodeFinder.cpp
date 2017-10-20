#include "GeodeFinder.h"

GeodeFinder::GeodeFinder ()
   : NodeVisitor (osg::NodeVisitor::TRAVERSE_ALL_CHILDREN) {}

void GeodeFinder::apply (osg::Node &searchNode) {
   if (! strcmp (searchNode.className(), "Geode")) {
      foundGeodes.push_back ((osg::Geode*) &searchNode);
   }

   traverse (searchNode);
}


std::vector<osg::Geode*> GeodeFinder::getNodeList() {
   return foundGeodes;
}


// utility function:
// set something for all geodes of a subgraph
// e.g.: culling off or set NodeMask for all geode children of a loaded model
void GeodeFinder::setGeodeDefaultSettings(osg::Node* node){
   this->apply(*node);

   std::vector<osg::Geode*> geodeList = this->getNodeList();

   if(geodeList.at(0)){
       for(int i = 0; i < geodeList.size(); i++){
           //GeodeSettings here:
           //culling off
           geodeList.at(i)->setCullingActive(false);

           //castShadowMask 0x41
           geodeList.at(i)->setNodeMask(0x0101);
       }
   }
}
