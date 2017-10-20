/*!
 * NodeVisitorCallbackRunner.h
 *
 *  Created on: Jan 17, 2014
 *     Authors: dmueller
 **/

#ifndef NVCR_H_
#define NVCR_H_

#include <osg/NodeVisitor>
#include <osg/Node>
#include <boost/optional.hpp>

/*!
 * @brief
 *
 * @description Use this class to run callbacks for each node and each geode. <br>
 * If a node callback returns true, the node will be traversed. <br>
 *  <CODE> <br>
 *      // set two callbacks <br>
 *      auto nodecallback = [] (osg::Node& node) { std::cout << "Found Node: " << node.getName() << "\n"; }; <br>
 *      auto geodecallback = [] (osg::Geode& geode) { std::cout << "Found Geode: " << geode.getName() << "\n"; }; <br>
 *      NodeVisitorCallbackRunner cbRunner(nodecallback, geodecallback); <br>
 *      root->accept(cbRunner); <br>
 *  </CODE>
 *  <CODE> <br>
 *      // run only callbacks for nodes <br>
 *      auto nodecallback = [] (osg::Node& node) { std::cout << "Found Node: " << node.getName() << "\n"; }; <br>
 *      NodeVisitorCallbackRunner cbRunner(nodecallback); <br>
 *      root->accept(cbRunner); <br>
 *  </CODE>
 *  <CODE> <br>
 *      // or pass lambda functions to set a member function instead of callbacks (must be binding variables!) <br>
 *      NodeVisitorCallbackRunner cbRunner([&] (osg::Geode& geode) { myObject.myFunction(geode); }); <br>
 *      root->accept(cbRunner); <br>
 *  </CODE>
 **/
class NodeVisitorCallbackRunner : public osg::NodeVisitor
{
    private:
        /*!
         * @brief optional callback function used for nodes. Will be false, if not set.
         **/
        boost::optional<std::function<bool (osg::Node& node)>>   _ncallback_function;
        
        /*!
         * @brief optional callback function used for geodes. Will be false, if not set.
         **/
         boost::optional<std::function<bool (osg::Geode& geode)>> _gcallback_function;

    public:
        /**
         * @brief Constructor, setting both callbacks
         * @details This will set the geode and node callback.
         * 
         * @param ncallback_function node callback function pointer
         * @param gcallback_function geode callback function pointer
         */
        NodeVisitorCallbackRunner(
            std::function<bool (osg::Node& node)>   ncallback_function,
            std::function<bool (osg::Geode& geode)> gcallback_function)
                : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
            _ncallback_function = ncallback_function;
            _gcallback_function = gcallback_function;
        }
        
        /**
         * @brief Constructor, setting only geode callback function
         * @details The node callback function will be set to false (using boost::optional)
         * 
         * @param gcallback_function geode callback function pointer
         */
        NodeVisitorCallbackRunner(
            std::function<bool (osg::Geode& geode)> gcallback_function)
                : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
            _gcallback_function = gcallback_function;
        }

        /**
         * @brief Constructor, setting only node callback function
         * @details The geode callback function will be set to false (using boost::optional)
         * 
         * @param ncallback_function node callback function pointer
         */
        NodeVisitorCallbackRunner(
            std::function<bool (osg::Node& node)> ncallback_function)
                : osg::NodeVisitor(osg::NodeVisitor::TRAVERSE_ALL_CHILDREN)
        {
            _ncallback_function = ncallback_function;
        }

        /**
         * @brief Run geode callback function (if set) for each geode
         * @details This function will be called automatically by the osg::NodeVisitor base class for each found geode.
         * 
         * @param geode the found geode
         */
        void apply(osg::Geode& geode)
        {
            if (_gcallback_function)
            {
                _gcallback_function.get()(geode); // get callback from boost and execute it
            }
        }

        /**
         * @brief Run node callback (if set) and further traverse node, if the node callback returns true
         * @details This function will be called automatically by the osg::NodeVisitor base class for each found node.
         * 
         * @param node the found node
         */
        void apply(osg::Node& node)
        {
            if (_ncallback_function)
            {
                if (_ncallback_function.get()(node)) // get function pointer and trigger ()-operator
                {
                    traverse(node);
                }
            }
        }
};

#endif