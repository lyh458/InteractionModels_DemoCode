#ifndef __XSITE_PLUGIN_TEMPLATE_H
#define __XSITE_PLUGIN_TEMPLATE_H

#if IMOD_CAVE
    #include <xsiteLib.h>
#else
    #include "../CaveDummy.h"
#endif

#include "BlenderController.h"
#include "../VirtualHuman.h"
#include "../VirtualHumanNao.h"
#include "../MyInteractionMeshController.h"
#include "../InteractionROI.h"

#include <osgShadow/ShadowedScene>
//#include "../Nao/Robot.h"

/**
 * @brief Wrapper Class for use in local AND cave/xsite-environment
 * @details Compilerflag IMOD_CAVE (set by CMakeLists.txt) defines whether cave/xsite-envirenment is used or not. \n
 *          Therefore there are multiple declarations and implementations of several methods encapsulated into if-else preprocessor-directives for conditional compilation.
 */
class CaveAndLocalWrapper: public xs::PluginBase
{
public:

#ifndef IMOD_CAVE
/**
* @brief (standard)-ctor for local use
*/
    CaveAndLocalWrapper();
/**
 * @brief local init, calls createScene() Method, sets some lighting options
 * @return true if succeeded
 */
    bool init();
#else
/**
* @brief default ctor of xs::PluginBase
* @param host
*/
    CaveAndLocalWrapper(xs::Host& host);
    bool init(const xs::ArgumentsList& l = xs::ArgumentsList());
#endif

    //! dtor
    ~CaveAndLocalWrapper();

#if IMOD_CAVE
/**
* update & interpreteEvent only used in cave/xs-environment
*/
    //! update the scene with new data
    void update();

    //! Interprete incomming events
    void interpreteEvent(xs::Event event, VirtualHuman* vh, const std::vector<std::string> rotationBoneNamesVec, const std::vector<std::string> translationBoneNamesVec);
#endif

    //! simulate the next step and produce events (if necessary)
    void simulate(double step = 0.0);

    MyInteractionMesh::MyInteractionMeshController *_meshController;
    
    osg::Vec3 getHeadPosition() { return this->headPosition; }

private:
    void createPitfall();
    //! should include the scenebuilding or the instantiation of the sceneobjects and adding them to the rootnode
    void createScene();

    //! loads and sets up virtual humans
    void createVHumans();

    //! loads and sets up Nao Rig and Model
    void createRobot();

    //! loads static objects (sky, ground plane) and sets up lighting
    void createStaticScene();

    //! loads static objects (and snow particle system) for christmas scene
    void createStaticChristmasScene();

    //! Loads a scene representing a kitchen
    void createKitchenScene();

    //! loads other dynamic objects (present), almost like createVHumans(), but seperated for organization purposes
    void createDynamicScene();

    //! sets up InteractionMesh
    void createInteractionMesh();

    /*!
    Function to fix received Blender rotations for the Nao robot to match the OSG coordinate system
    */
    void fixReceivedRobotRotations(std::vector<osg::Quat> &rotations, std::vector<std::string> &rotationBoneNamesVec, int &k);

    //! Update a virtual humans posture
    void updatePosture(VirtualHuman *vhuman, iMod::BlenderController *blend, std::vector<std::string> positionBoneNamesVec, std::vector<std::string> rotationBoneNamesVec, std::string threadName, double playbacktime );

    //void updatePosture(Robot *robot, iMod::BlenderController *blend, double playbackTime);

    /* attributes used in demo */
    double _val;
    double _inc;
    
    /* HeadPosition */
    osg::Vec3 headPosition;

    /*Vector with all Blender controllers, e.g. Blender connections*/
    std::vector<iMod::BlenderController*> _blenderController;


    bool _isBlenderControllerInitialized;

    VirtualHuman* _vh1;
    VirtualHuman* _vh2;
    VirtualHuman* _vh3;

    VirtualHuman* _vhObject;

    VirtualHumanNao* _vhNaoTube;
    VirtualHumanNao* _vhNaoMesh;

    osg::ref_ptr<osg::Switch> _naoSwitchNode;

    bool _running;

    osg::ref_ptr<osgShadow::ShadowedScene> _shadowRoot;
    osg::ref_ptr<osg::PositionAttitudeTransform> _rootPAT;
    InteractionROI *_roiRegion;

    const static int _receiveShadowMask = 0x1011;
    const static int _castShadowMask = 0x0101;

};

#if IMOD_CAVE
/* this function replaces the old factory      */
/* this is the entry point for the XSiteCluser */
extern "C"
xs::PluginBase* xs_create_instance(xs::Host & host)
{
    return new CaveAndLocalWrapper(host);
}

#endif
#endif
