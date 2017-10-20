#include <osg/Array>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/Geode>
#include <osgAnimation/Bone>
#include <string>

/**
 * @brief Finds recursivly a node in a scenegraph
 * @param searchName the name of the bone which is wanted
 * @param currNode the node under which the search begins
 * @return the pointer to the wanted node
 */
osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);
/**
 * @brief Explodes a string into a parts vector, parts are limited by a delimiter char
 * @param str string to explode
 * @param ch the delimiter
 * @return parted strings in a vector
 */
std::vector<std::string> explode(const std::string& str, const char& ch);


struct AddHelperBone : public osg::NodeVisitor{
    AddHelperBone(bool draw = false) : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN){}
    void apply(osg::Transform& node) {
        osgAnimation::Bone* bone = dynamic_cast<osgAnimation::Bone*>(&node);
        if (bone)
            bone->addChild(createAxis());
        traverse(node);
    }
    
    osg::Geode* createAxis(){
        osg::Geode*     geode    = new osg::Geode();
        osg::Geometry*  geometry = new osg::Geometry();
        osg::Vec3Array* vertices = new osg::Vec3Array();
        osg::Vec4Array* colors   = new osg::Vec4Array();

        vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
        vertices->push_back(osg::Vec3(1.0f, 0.0f, 0.0f));
        vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
        vertices->push_back(osg::Vec3(0.0f, 1.0f, 0.0f));
        vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
        vertices->push_back(osg::Vec3(0.0f, 0.0f, 1.0f));

        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        colors->push_back(osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 1.0f, 0.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));
        colors->push_back(osg::Vec4(0.0f, 0.0f, 1.0f, 1.0f));

        geometry->setVertexArray(vertices);

    	//geometry->setColorArray(colors, osg::Array::BIND_PER_VERTEX);
    	geometry->setColorArray(colors);
    	geometry->setColorBinding (osg::Geometry::BIND_PER_VERTEX);

        geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 6));
        geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, false);

        geode->addDrawable(geometry);

        return geode;
    }
};
