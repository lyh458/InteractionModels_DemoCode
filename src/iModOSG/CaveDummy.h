
#include <osg/Node>
#include <osg/Group>

namespace xs{

class PluginBase {

public:
/**
* @brief Dummy-ctor-Implementation of xs::PluginBase for local use (without xs-framework)
*/
	PluginBase(){
		_root = new osg::Group;
	}

/**
 * @brief Default root for local scene
 */
	osg::ref_ptr<osg::Group> _root;
};

/**
 * @brief Dummy Definition of xs::Host
 */
class Host;

/**
 * @brief Dummy Definition of xs::ArgumentList
 */
class ArgumentsList;

}
