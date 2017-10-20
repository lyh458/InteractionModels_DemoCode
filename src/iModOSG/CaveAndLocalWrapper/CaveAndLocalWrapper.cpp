#include "CaveAndLocalWrapper.h"

#include <osg/Array>
#include <osg/Geometry>
#include <osg/Group>
#include <osg/PositionAttitudeTransform>
#include <osg/ShapeDrawable>
#include <osg/View>
#include <osg/Fog>
#include <osg/ClearNode>

#include <osgParticle/PrecipitationEffect>

#include <osgAnimation/BasicAnimationManager>
#include <osgAnimation/Bone>
#include <osgAnimation/UpdateBone>
#include <osgAnimation/StackedRotateAxisElement>
#include <osgAnimation/StackedTranslateElement>
#include <osgAnimation/Skeleton>

#include <osgDB/FileUtils>
#include <osgDB/Options>
#include <osgDB/ReadFile>

#include <osgGA/DriveManipulator>
#include <osgGA/FlightManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgGA/MultiTouchTrackballManipulator>
#include <osgGA/SphericalManipulator>
#include <osgGA/StateSetManipulator>
#include <osgGA/TerrainManipulator>
#include <osgGA/TrackballManipulator>

#include <osgUtil/Optimizer>

//#ifndef IMOD_CAVE
    #include <osgShadow/ShadowedScene>
    #include <osgShadow/ViewDependentShadowMap>
    #include <osgShadow/StandardShadowMap>
    #include <osgShadow/ShadowMap>
    #include <osgShadow/SoftShadowMap>
    #include <osgShadow/ParallelSplitShadowMap>
    #include <osgShadow/MinimalShadowMap>

    #include "../ownViewDependentShadowMap.h"
//#endif

#include <osg/Light>
#include <osg/LightSource>

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>

#include <iostream>
#include <vector>
#include <map>

#include "../BoneDefinitions.h"
#include "../HelperClassesAndFunctions.h"

struct BoneRotation{
    osg::Vec3 axis1 = osg::Vec3(1,0,0);
    osg::Vec3 axis2 = osg::Vec3(1,0,0);;
    
    int direction1 = 1;
    int direction2 = 1;
    
} myBoneRotationFix;

/* \cond */

struct MyCameraPostDrawCallback : public osg::Camera::DrawCallback
{
    ownViewDependentShadowMap* _vdsm;

    MyCameraPostDrawCallback(ownViewDependentShadowMap* vdsm):
        _vdsm(vdsm)
    {}

    virtual void operator () (const osg::Camera& /*camera*/) const
    {
        osg::StateSet* state = _vdsm->getVDDState();

        if(!state){
            std::cout<<"State = NULL"<< std::endl;
        }
        else{
            //std::cout<<"StateName: "<<state->getName()<< std::endl;

            state->clear();

                            state->setTextureMode(1,GL_TEXTURE_GEN_S,osg::StateAttribute::OFF);
                            state->setTextureMode(1,GL_TEXTURE_GEN_T,osg::StateAttribute::OFF);
                            state->setTextureMode(1,GL_TEXTURE_GEN_R,osg::StateAttribute::OFF);
                            state->setTextureMode(1,GL_TEXTURE_GEN_Q,osg::StateAttribute::OFF);

        }

    }
};

class ShowNodeNamesVisitor : public osg::NodeVisitor
{
public:
    ShowNodeNamesVisitor( const bool showGeodes )
      : osg::NodeVisitor( osg::NodeVisitor::TRAVERSE_ALL_CHILDREN ),
        _level( 0 ),
        _showGeodes( showGeodes )
    {}

    void apply( osg::Node& n )
    {
        display( n.getName(), n.className() );

        _level++;
        traverse( n );
        _level--;
    }

    void apply( osg::Geode& n )
    {
        if( !_showGeodes )
            return;

        // For Geodes, along with className, also include number of Drawables.
        std::ostringstream ostr;
        const unsigned int nd = n.getNumDrawables();
        ostr << n.className() << " with " << nd << " Drawable" << ((nd!=1)?"s":"");
        display( n.getName(), ostr.str() );

        // No traverse for Geodes (they have no children, so it's a no-op).
    }

protected:
    void display( const std::string& name, const std::string& classInfo )
    {
        int idx;
        for( idx=0; idx<_level; idx++ )
            osg::notify( osg::NOTICE ) << "  ";

        osg::notify( osg::NOTICE ) << ( ( !name.empty() ) ? name : "NULL" );
        osg::notify( osg::NOTICE ) << " (" << classInfo << ")" << std::endl;
    }

    int _level;
    bool _showGeodes;
};

#ifndef IMOD_CAVE
CaveAndLocalWrapper::CaveAndLocalWrapper()
{

}

bool CaveAndLocalWrapper::init()
{
    /* for the CaveAndLocalWrapper we just create a scene */
    createScene();
    /* there is not much that could go wrong here... */
    return true;
}
#else

CaveAndLocalWrapper::CaveAndLocalWrapper(xs::Host& host)
    : PluginBase(host)
{
    /* you may give your plugin a more meaningful name */
    _name = "iModOSG";

    _isBlenderControllerInitialized = false;
    /* demo for the notify-system...refer to lib/include/Notify.h to see more about the NOTIFY-levels */
    /* current level is xs::DEBUG, defined in XSiteCluster.cpp */
    XS_NOTIFY(xs::INFO,"Plugin: " << _name);

    _inc = 0;

    /* We define two special events here. They start & stop the animation of the cube/cow.
     * The management-tool will show two buttons, which fire the corresponding value.*/
    xs::Event ev(xs::Event::CUSTOM_TYPE, (int) 1, xs::Event::BROADCAST, "ToggleInteractionMesh");
    registerEvent(ev);

    xs::Event ev2(xs::Event::CUSTOM_TYPE, (int) 1, xs::Event::BROADCAST, "ToggleNaoShape");
    registerEvent(ev2);

}

bool CaveAndLocalWrapper::init(const xs::ArgumentsList&)
{
    /* for the CaveAndLocalWrapper we just create a scene */
    createScene();
    //osg::StateSet* rootState = _root->getOrCreateStateSet();
    //rootState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    //_root->setStateSet(rootState);

    /* there is not much that could go wrong here... */
    return true;
}

#endif

CaveAndLocalWrapper::~CaveAndLocalWrapper()
{

}

void CaveAndLocalWrapper::createStaticScene(){
    _rootPAT  = new osg::PositionAttitudeTransform;
    //osg::ref_ptr<osg::Node> checkerBoard = osgDB::readNodeFile("objects/checkerboard_metric.obj");
    osg::ref_ptr<osg::Node> checkerBoard = osgDB::readNodeFile("objects/whitePlane.obj");
    checkerBoard->setName("GroundCheckerBoard");
    checkerBoard->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);


    //Sky
    // osg::ref_ptr<osg::Node> sky = osgDB::readNodeFile("objects/sky_check.obj");
    // osg::StateSet* skyState = sky->getOrCreateStateSet();
    // skyState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    // sky->setStateSet(skyState);
    // sky->setName("Sky");
    //  _root->addChild(sky);

    //Load table
    //osg::ref_ptr<osg::Node> table = osgDB::readNodeFile("roundTable/roundTable.osgt");
    //osg::ref_ptr<osg::PositionAttitudeTransform> tablePat = new osg::PositionAttitudeTransform;
    //tablePat->setPosition(osg::Vec3(2.0,0.78,-1.276));
    //tablePat->addChild(table);

    //Load Ground Plate
    //osg::ref_ptr<osg::Node> plate = osgDB::readNodeFile("objects/plate.obj");
    //osg::ref_ptr<osg::PositionAttitudeTransform> platePat = new osg::PositionAttitudeTransform;
    //platePat->setPosition(osg::Vec3(0,0,0));
    //platePat->setName("InteractionROIPlate");
    //platePat->addChild(plate);

    //_roiRegion = new InteractionROI();

#if IMOD_CAVE
    //_root->addChild(lightSource);

    //Objects
    //_root->addChild(tablePat)
    //_root->addChild(platePat);
    _root->addChild(checkerBoard);
    _root->addChild(_rootPAT);

#else
    osg::LightSource * lightSource = new osg::LightSource();
    osg::Light * light = new osg::Light();
    light->setLightNum(0);
    light->setPosition(osg::Vec4(0,-4,3,0));
    light->setAmbient(osg::Vec4(0.1,0.1,0.1,1));
    light->setDiffuse(osg::Vec4(0.8,0.8,0.8,1));
    light->setDirection(osg::Vec3(0,0,0));
    light->setConstantAttenuation(0.1f);
    lightSource->setLight(light);
    lightSource->setLocalStateSetModes(osg::StateAttribute::ON);

    osg::ref_ptr<ownViewDependentShadowMap> vdsm = new ownViewDependentShadowMap;

    _shadowRoot = new osgShadow::ShadowedScene;
    _shadowRoot->addChild(lightSource);

    _shadowRoot->setShadowTechnique( vdsm.get() );

    // vdsm->getShadowedScene()->getShadowSettings()->setDebugDraw(true);
    vdsm->getShadowedScene()->getShadowSettings()->setShaderHint(osgShadow::ShadowSettings::PROVIDE_FRAGMENT_SHADER);
    vdsm->getShadowedScene()->getShadowSettings()->setTextureSize(osg::Vec2s(4096,4096));

    vdsm->dirty();
    vdsm->cleanSceneGraph();

    _shadowRoot->setReceivesShadowTraversalMask( _receiveShadowMask );
    _shadowRoot->setCastsShadowTraversalMask( _castShadowMask );

    //Floor
    checkerBoard->setNodeMask(_receiveShadowMask);
    _shadowRoot->addChild(checkerBoard);
    //_shadowRoot->addChild(tablePat);
    //_shadowRoot->addChild(platePat);

    //_roiRegion->add(_shadowRoot);

    _shadowRoot->addChild(_rootPAT);
    _root->addChild(_shadowRoot);

    //Kitchen
    // osg::ref_ptr<osg::Node> kitchen = osgDB::readNodeFile("objects/kitchen.obj");
    // _rootPAT->addChild(kitchen);
    // kitchen->getOrCreateStateSet()->setMode(GL_LIGHTING, osg::StateAttribute::OFF);

#endif

// #ifdef IMOD_CAVE
//         std::vector<osg::Camera*> camlist;

//         //AndereViewer Probieren
//         getHost().getViewerBase().getMasterViewer()->getCameras(camlist);

//         osg::Camera *cam = camlist.at(0);
//         std::cout<<"CamListSize: "<<camlist.size()<<std::endl;

//         cam->setPostDrawCallback(new MyCameraPostDrawCallback(vdsm));

//         //xsite StereoViewer überschreiben-> setPostDrawCallback

// #endif

    //#endif

    // //ClearNode might solve CAVE/Mersive-Problems???
    // osg::ref_ptr<osg::ClearNode> clearNode = new osg::ClearNode;
    // clearNode->setClearColor(osg::Vec4(1,1,1,1));
    // clearNode->setClearMask(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
    // _root->addChild(clearNode);
}

void CaveAndLocalWrapper::createDynamicScene(){

    //Create additional objects as virtual humans
    osg::ref_ptr<osg::Group> vhBoxGroup = new osg::Group;
    osg::ref_ptr<osg::PositionAttitudeTransform>_vhObjectPAT = new osg::PositionAttitudeTransform;
    _vhObjectPAT->setName("objectTrans");
    osg::Quat defautlRotat = osg::Quat(0.0,0.0,0.0,1.0);
    osg::Vec3 defaultTrans = osg::Vec3(0.0,0,0.0);
    _rootPAT->addChild(_vhObjectPAT);
    _vhObjectPAT.get()->addChild(vhBoxGroup);
    _vhObject = new VirtualHuman();
    _vhObject->load("objects/Axis.osgt");
    _vhObject->setup(vhBoxGroup,defaultTrans,defautlRotat);
}

void CaveAndLocalWrapper::createVHumans(){

    //Creating virtual human 1
    osg::ref_ptr<osg::Group> vh1Group = new osg::Group;
    osg::ref_ptr<osg::PositionAttitudeTransform> _vh1Trans = new osg::PositionAttitudeTransform;
    _vh1Trans->setName("vh1Trans");
    osg::Quat defautlRotat(0.0, 0.0, 0.0, 1.0);
    osg::Vec3 defaultTrans(0.0, 0, 0.0);
    _rootPAT->addChild(_vh1Trans);
    _vh1Trans.get()->addChild(vh1Group);
    _vh1 = new VirtualHuman();
    //_vh1->load("VirtualHumans/iTaSC_Skeleton/WoodyBlue.fbx");
    //_vh1->load("VirtualHumans/iTaSC_Skeleton/Beta.fbx");
    _vh1->load("VirtualHumans/BEPUik_Skeleton/Alpha_182cm.osgb");
    //_vh1->load("VirtualHumans/BEPUik_Skeleton/Beta_182cmArm.osgt");

    //_vh1->load("VirtualHumans/BEPUik_Skeleton/BetaKinect.fbx");
    _vh1->setup(vh1Group,defaultTrans, defautlRotat);

    //Creating virtual human 2
    osg::ref_ptr<osg::Group> vh2Group = new osg::Group;
    osg::ref_ptr<osg::PositionAttitudeTransform>_vh2Trans = new osg::PositionAttitudeTransform;
    _vh2Trans->setName("vh2Trans");
    defautlRotat = osg::Quat(0.0,0.0,0.0,1);
    defaultTrans = osg::Vec3(0,0,0);
    _rootPAT->addChild(_vh2Trans);
    _vh2Trans.get()->addChild(vh2Group);
    _vh2 = new VirtualHuman();
    //_vh2->load("VirtualHumans/iTaSC_Skeleton/UR5WithHand.osgt");
    //_vh2->load("VirtualHumans/BEPUik_Skeleton/UR5.osgt");
     _vh2->load("VirtualHumans/BEPUik_Skeleton/Alpha_182cm.osgb");
    //_vh2->load("VirtualHumans/iTaSC_Skeleton/Alpha.fbx");
    //_vh2->load("VirtualHumans/iTaSC_Skeleton/WoodyRed.fbx");
     _vh2->setup(vh2Group,defaultTrans,defautlRotat);

    //Creating robot arm
    // osg::ref_ptr<osg::Group> vh3Group = new osg::Group;
    // osg::ref_ptr<osg::PositionAttitudeTransform>_vh3Trans = new osg::PositionAttitudeTransform;
    // _vh3Trans->setName("vh3Trans");
    // defautlRotat = osg::Quat(0.0,0.0,0.0,1);
    // defaultTrans = osg::Vec3(0,0,0);
    // _rootPAT->addChild(_vh3Trans);
    // _vh3Trans.get()->addChild(vh3Group);
    // _vh3 = new VirtualHuman();

    // //_vh3->load("VirtualHumans/iTaSC_Skeleton/UR5WithHand.osgt");
    // _vh3->load("VirtualHumans/BEPUik_Skeleton/UR5.osgt");
    // _vh3->setup(vh3Group,defaultTrans,defautlRotat);
}

void CaveAndLocalWrapper::createInteractionMesh(){
    std::vector<osg::Node*> nodeVec;

    nodeVec.push_back(findNamedNode("vh1Trans",_rootPAT));
    nodeVec.push_back(findNamedNode("vh2Trans",_rootPAT));
    // nodeVec.push_back(_vh3Trans);

    std::vector<std::vector<std::string>> boneNameVec;
    boneNameVec.push_back(boneNamesInteractionMesh); // boneNamesInteractionMesh -> See BoneDefinitions.h
    boneNameVec.push_back(boneNamesInteractionMesh);
    // boneNameVec.push_back(boneNamesInteractionMeshObject);

    _meshController = new MyInteractionMesh::MyInteractionMeshController(MyInteractionMesh::MyMeshKind::LINE, _rootPAT);
    _meshController->readConnectionVectorFromFile(std::string(iMODOSG_DATA_DIR)+"../tmp/meshReference.matmeshTriangulation", nodeVec, boneNameVec);
    _meshController->addMeshToSceneGraph(_rootPAT);
}

void CaveAndLocalWrapper::createRobot(){

    //Create Nao virtual human
	_naoSwitchNode = new osg::Switch;
	osg::ref_ptr<osg::Group> meshGroup = new osg::Group;
	osg::ref_ptr<osg::Group> tubeGroup = new osg::Group;
	osg::ref_ptr<osg::PositionAttitudeTransform> _naoTrans = new osg::PositionAttitudeTransform;
	_naoTrans->setName("naoTrans");
    osg::Quat defautlRotat = osg::Quat(0.0,0.0,0.0,1.0);
	osg::Vec3 defaultTrans = osg::Vec3(0.0,0.0,0.0);//osg::Vec3(2.2,0.65,0.02);
    _rootPAT->addChild(_naoTrans);
	_naoTrans.get()->addChild(_naoSwitchNode);

	_vhNaoMesh = new VirtualHumanNao();
	_vhNaoTube = new VirtualHumanNao();

	_naoSwitchNode->addChild(meshGroup);
	_naoSwitchNode->addChild(tubeGroup);

	_vhNaoMesh->loadNaoMesh();
	_vhNaoTube->loadNaoTube();

	_vhNaoMesh->setup(meshGroup, defaultTrans, defautlRotat);
	_vhNaoTube->setup(tubeGroup, defaultTrans, defautlRotat);

	_naoSwitchNode->setSingleChildOn(0);
	_naoSwitchNode->setEventCallback(new SwitchNaoKeyboardEventHandler);
}

void CaveAndLocalWrapper::createStaticChristmasScene(){
    ////
    //Christmas Scene

    osg::PositionAttitudeTransform* christmasPAT = new osg::PositionAttitudeTransform();

    osg::Node* floorGeom = osgDB::readNodeFile("objects/ChristmasScene/floor.obj");
    osg::Node* sky = osgDB::readNodeFile("objects/ChristmasScene/sky.obj");
    osg::Node* lantern = osgDB::readNodeFile("objects/ChristmasScene/laterne.obj");
    osg::Node* house = osgDB::readNodeFile("objects/ChristmasScene/treehouse.obj");

    osg::Node* treeLeaves1 = osgDB::readNodeFile("objects/ChristmasScene/tree_leaves.obj");
    osg::Node* treeLeaves2 = osgDB::readNodeFile("objects/ChristmasScene/tree_leaves2.obj");
    osg::Node* treeLeaves3 = osgDB::readNodeFile("objects/ChristmasScene/tree_leaves3.obj");
    osg::Node* treeLeaves4 = osgDB::readNodeFile("objects/ChristmasScene/tree_leaves4.obj");

    osg::Node* treeTrunk1 = osgDB::readNodeFile("objects/ChristmasScene/tree_trunk.obj");
    osg::Node* treeTrunk2 = osgDB::readNodeFile("objects/ChristmasScene/tree_trunk2.obj");
    osg::Node* treeTrunk3 = osgDB::readNodeFile("objects/ChristmasScene/tree_trunk3.obj");
    osg::Node* treeTrunk4 = osgDB::readNodeFile("objects/ChristmasScene/tree_trunk4.obj");

    osg::Node* geschenk = osgDB::readNodeFile("objects/ChristmasScene/Geschenk.obj");
    osg::Node* geschenk2 = osgDB::readNodeFile("objects/ChristmasScene/Geschenk2.obj");


    osg::StateSet* leaveState1 = treeLeaves1->getOrCreateStateSet();
    leaveState1->setMode(GL_BLEND, osg::StateAttribute::ON);
    leaveState1->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    treeLeaves1->setStateSet(leaveState1);

    osg::StateSet* leaveState2 = treeLeaves2->getOrCreateStateSet();
    leaveState2->setMode(GL_BLEND, osg::StateAttribute::ON);
    leaveState2->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    treeLeaves2->setStateSet(leaveState2);

    osg::StateSet* leaveState3 = treeLeaves3->getOrCreateStateSet();
    leaveState3->setMode(GL_BLEND, osg::StateAttribute::ON);
    leaveState3->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    treeLeaves3->setStateSet(leaveState3);

    osg::StateSet* leaveState4 = treeLeaves4->getOrCreateStateSet();
    leaveState4->setMode(GL_BLEND, osg::StateAttribute::ON);
    leaveState4->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    treeLeaves4->setStateSet(leaveState4);

    christmasPAT->addChild(floorGeom);
    christmasPAT->addChild(sky);
    christmasPAT->addChild(lantern);
    christmasPAT->addChild(house);
    christmasPAT->addChild(treeTrunk1);
    christmasPAT->addChild(treeTrunk2);
    christmasPAT->addChild(treeTrunk3);
    christmasPAT->addChild(treeTrunk4);
    christmasPAT->addChild(geschenk);
    christmasPAT->addChild(geschenk2);
    christmasPAT->addChild(treeLeaves1);
    christmasPAT->addChild(treeLeaves2);
    christmasPAT->addChild(treeLeaves3);
    christmasPAT->addChild(treeLeaves4);

    //christmasPAT->setScale(osg::Vec3(100,100,100));
    christmasPAT->getOrCreateStateSet();
    _root->addChild(christmasPAT);

    osg::ref_ptr<osgParticle::PrecipitationEffect> sno = new osgParticle::PrecipitationEffect;
    sno->snow(0.1);
    sno->setParticleSize(0.075f);
    sno->setNearTransition(0.5f);

    _root->addChild(sno.get());

    osg::StateSet* rootState = _root->getOrCreateStateSet();
    rootState->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    _root->setStateSet(rootState);
}

void CaveAndLocalWrapper::createPitfall()
{
	std::string path = "/vol/vrrender/data/PitFall/";
	osg::Node *loadedModel = osgDB::readNodeFile(path + "OBJ/treppe_ohne_extras.obj");
    osg::Node *baum = osgDB::readNodeFile(path + "OBJ/baum.obj");
    osg::Node *ground = osgDB::readNodeFile(path + "OBJ/hintergrund.obj");
    osg::Node *wandlinks = osgDB::readNodeFile(path + "OBJ/wandlinks.obj");
    
    osg::Node *gelaender = osgDB::readNodeFile(path + "OBJ/gelaender.obj");
    osg::Node *knauf = osgDB::readNodeFile(path + "OBJ/knauf.obj");
    osg::Node *glass = osgDB::readNodeFile(path + "OBJ/glass.obj");
    osg::Node *efeunode = osgDB::readNodeFile(path + "OBJ/efeu.obj");
    osg::StateSet *ss_efeu = efeunode->getOrCreateStateSet();
    ss_efeu->setMode(GL_BLEND, osg::StateAttribute::ON);
    ss_efeu->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
    efeunode->setStateSet(ss_efeu);
    osg::Image *image_refl = osgDB::readImageFile(path + "Bake/bake_refl.png");
    osg::Image *image_refl_fenster = osgDB::readImageFile(path + "Bake/bake_refl_fenster.png");

    osg::Texture2D *tex_refl = new osg::Texture2D();
    tex_refl->setImage(image_refl);
    osg::Texture2D *tex_refl_fenster = new osg::Texture2D();
    tex_refl_fenster->setImage(image_refl_fenster);
    osg::TexGen *texgen = new osg::TexGen();
    texgen->setMode(osg::TexGen::SPHERE_MAP);
    osg::TexGen *texgen_fenster = new osg::TexGen();
    texgen_fenster->setMode(osg::TexGen::REFLECTION_MAP);
    osg::StateSet *ss_gelaender = gelaender->getOrCreateStateSet();
    osg::StateSet *ss_knauf = knauf->getOrCreateStateSet();
    osg::StateSet *ss_glass = glass->getOrCreateStateSet();
    ss_gelaender->setTextureAttributeAndModes(1, tex_refl, osg::StateAttribute::ON);
    ss_gelaender->setTextureAttributeAndModes(1, texgen, osg::StateAttribute::ON);
    ss_knauf->setTextureAttributeAndModes(1, tex_refl, osg::StateAttribute::ON);
    ss_knauf->setTextureAttributeAndModes(1, texgen, osg::StateAttribute::ON);
    ss_glass->setTextureAttributeAndModes(1, tex_refl_fenster, osg::StateAttribute::ON);
    ss_glass->setTextureAttributeAndModes(1, texgen_fenster, osg::StateAttribute::ON);
    gelaender->setStateSet(ss_gelaender);
    knauf->setStateSet(ss_knauf);
    glass->setStateSet(ss_glass);
    baum->setStateSet(ss_efeu);
    glass->setStateSet(ss_efeu);

    _root->addChild(loadedModel);
    _root->addChild(wandlinks);
    _root->addChild(ground);
    _root->addChild(gelaender);
    _root->addChild(knauf);
    _root->addChild(glass);
    _root->addChild(baum);
    _root->addChild(efeunode);
    osg::StateSet *ss_szene = _root->getOrCreateStateSet();
    ss_szene->setMode(GL_LIGHTING, osg::StateAttribute::OFF);
    _root->setStateSet(ss_szene);
}

void CaveAndLocalWrapper::createScene()
{
    // add iMODOSG data path to osg file path..
    osgDB::FilePathList datapath = osgDB::getDataFilePathList();
    datapath.push_front(std::string(iMODOSG_DATA_DIR));
    // evtl. noch einen Datenpfad für CAVE definieren?
    osgDB::setDataFilePathList(datapath);

    createStaticScene();
    //createPitfall();

    //createStaticChristmasScene();

    //createDynamicScene();
    createVHumans();
	//createRobot();
    createInteractionMesh();

    osgUtil::Optimizer *opti = new osgUtil::Optimizer();
    opti->optimize(_root);

    //SceneGraph Output
    unsigned int mask( 0xffffffff );
    bool showGeodes( true );
    ShowNodeNamesVisitor snnv( showGeodes );
    snnv.setTraversalMask( mask );
    snnv.setNodeMaskOverride( 0xffffffff );
    _root->accept( snnv );

}


void CaveAndLocalWrapper::simulate(double step)
{
    if(!_isBlenderControllerInitialized){
        _isBlenderControllerInitialized = true;
        std::string blenderHost = iMod::Configuration::getInstance().getParameter<string>("iModOSG","BlenderHost");

        for(int i=0;i<2;++i){
            _blenderController.push_back(new iMod::BlenderController());
            _blenderController[i]->initialize(59919+i, blenderHost);
        }

        //Human Agent Interaction
        boost::thread readBlenderVHuman1(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[0], rotationBonesPhys.size() * 4 + 3 + 4));
        boost::thread readBlenderVHuman2(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[1], rotationBonesPhysNoHands.size() * 4 + 3 + 4));
        //boost::thread readBlenderVHuman3(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[2], rotationBonesUR5.size() * 4 + 3 + 4));


        //Human Robot Interaction
        //boost::thread readBlenderVHuman1(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[0], rotationBonesPhys.size() * 4 + 3 + 4));
        //boost::thread readBlenderVHuman2(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[1], rotationBonesUR5.size() * 4 + 3 + 4));

        // boost::thread readBlenderVHuman3(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[2], rotationBones.size() * 4 + 3 + 4));
        // boost::thread readBlenderVHuman4(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[3], rotationBones.size() * 4 + 3 + 4));
        // boost::thread readBlenderNao(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[4], naoRotationBones.size() * 4 + naoTranslationBones.size() * 3  + 4));

        //boost::thread readBlenderVHumanObject(boost::bind(&iMod::BlenderController::readBlenderPoses, _blenderController[2], rotationObjectBones.size() * 4 + 3 + 4));

        iMod::Configuration::getInstance().startCyclicRead();

    }

    //double _playbacktime1 = _blenderController[0]->getReadingFramerateEstimate();
    //double _playbacktime2 = _blenderController[1]->getReadingFramerateEstimate();
    
    double _playbacktimeAdditional = iMod::Configuration::getInstance().getParameter<double>("iModOSG","animationDurationOffset");
    double _playbacktimeRobot = iMod::Configuration::getInstance().getParameter<double>("iModOSG","animationDurationOffsetRobot");

    // Human Agent Interaction
    // --> Update virtual humans
    updatePosture(_vh1,_blenderController[0],translationBones,rotationBonesPhys,"VH_1_POSTURE",_playbacktimeAdditional);
    updatePosture(_vh2,_blenderController[1],translationBones,rotationBonesPhysNoHands,"VH_2_POSTURE",_playbacktimeAdditional);
    //updatePosture(_vh3,_blenderController[2],translationBones,rotationBonesUR5,"ROBOT_POSTURE",_playbacktimeRobot);

    // Human Robot Interaction
    // --> Update virtual humans
    //updatePosture(_vh1,_blenderController[0],translationBones,rotationBonesPhys,"VH_1_POSTURE",_playbacktimeAdditional);
    //updatePosture(_vh2,_blenderController[1],translationBones,rotationBonesUR5,"VH_2_POSTURE",_playbacktimeAdditional);

    //updatePosture(_vh2,_blenderController[1],translationBones,rotationBonesPhys,"VH_2_POSTURE",_playbacktime2+_playbacktimeAdditional);
    

    //updatePosture(_vh3,_blenderController[2],translationBones,rotationBones,"VH_3_POSTURE",playbacktime);
    // updatePosture(_vh4,_blenderController[3],translationBones,rotationBones,"VH_4_POSTURE",playbacktime);

    //Update Nao(s), we need two update functions since meshes and tubes want to be updated
    //updatePosture(_vhNaoMesh,_blenderController[2], naoTranslationBones, naoRotationBones, "NAO_POSTURE",playbacktime);
    //updatePosture(_vhNaoTube,_blenderController[2], naoTranslationBones, naoRotationBones, "NAO_POSTURE",playbacktime);

    //updatePosture(_vhObject,_blenderController[2],translationObjectBones,rotationObjectBones,"OBJECT_POSTURE",_playbacktimeAdditional);

#ifndef IMOD_CAVE
    _meshController->updateMesh();
#endif

}

#if IMOD_CAVE
void CaveAndLocalWrapper::update()
{
    while( !_incoming_events.empty() )
    {
        /* take the first event and pop it out of the queue */
        xs::Event event = _incoming_events.front();
        _incoming_events.pop();

        if( event.getEventType() == xs::Event::CUSTOM_TYPE )
        {
            std::string name = event.getEventName();
            //XS_NOTIFY(xs::INFO,"Processing update event: " << name);
            if (event.getEventName().compare("VH_1_POSTURE") == 0)
                interpreteEvent(event,_vh1, rotationBonesPhys,  translationBones);

            if (event.getEventName().compare("VH_2_POSTURE") == 0)
                interpreteEvent(event,_vh2, rotationBonesPhysNoHands,  translationBones);
            
            if (event.getEventName().compare("ROBOT_POSTURE") == 0)
                interpreteEvent(event,_vh3, rotationBonesUR5,  translationBones);

            if (event.getEventName().compare("NAO_POSTURE") == 0){
                interpreteEvent(event,_vhNaoMesh, naoRotationBones, naoTranslationBones);
                interpreteEvent(event,_vhNaoTube, naoRotationBones, naoTranslationBones);
            }
            if (event.getEventName().compare("ToggleInteractionMesh") == 0)
                _meshController->switchMode();

            if (event.getEventName().compare("ToggleNaoShape") == 0){
                if(_naoSwitchNode->getValue(0)){
                    _naoSwitchNode->setAllChildrenOff();
                    _naoSwitchNode->setValue(1,true);
                }
                else{
                    _naoSwitchNode->setAllChildrenOff();
                    _naoSwitchNode->setValue(0,true);
                }
            }
        }
    }
    //_meshController->updateMesh();
}

void CaveAndLocalWrapper::interpreteEvent(xs::Event event, VirtualHuman* vh, const std::vector<std::string> rotationBoneNamesVec, const std::vector<std::string> translationBoneNamesVec){
    // get the Value (you have to know the type of the data)
    xs::Binary _val;

    if (event.getValue(&_val))
    {

        int j = rotationBoneNamesVec.size();
        int k = translationBoneNamesVec.size();

        //! The new posture
        std::vector<double> rotationTimes;
        std::vector<osg::Quat> rotations;

        //! New jont positions
        osg::Vec3 rootPosition;
        osg::Quat rootRotation;
        double playbacktime;

        rotationTimes.resize(j);
        rotations.resize(j);

        //Pipe rotations timings
        for(int i=0;i<j;i++){
            double tmpTime;
            _val >> tmpTime;
            rotationTimes[j-i-1]=tmpTime;
        }

        //Pipe rotations
        for(int i=0;i<j;i++){
            double x,y,z,w;
            _val >> w;
            _val >> z;
            _val >> y;
            _val >> x;
            osg::Quat tmpQuat(x,y,z,w);
            rotations[j-i-1]=tmpQuat;
            // std::cout<<"Newrotationsx:"<<tmpQuat.x()<<std::endl;
            // std::cout<<"Newrotationsy:"<<tmpQuat.y()<<std::endl;
            // std::cout<<"Newrotationsz:"<<tmpQuat.z()<<std::endl;
            // std::cout<<"Newrotationsw:"<<tmpQuat.w()<<std::endl;
            //std::cout<<"NewRot"<<j-i-1<<": "<<rotations[j-i-1].x()<<";"<<rotations[j-i-1].y()<<";"<<rotations[j-i-1].z()<<";"<<rotations[j-i-1].w()<<std::endl;
        }

        //Pipe double playbacktime
        _val >> playbacktime;


        //Pipe rootRotation
        double w, x, y, z;
        _val >> w;
        _val >> z;
        _val >> y;
        _val >> x;
        rootRotation = osg::Quat(x, y, z, w);
        //std::cout<<"RootRotNew: "<<rootRotation.w()<<";"<<rootRotation.x()<<";"<<rootRotation.y()<<";"<<rootRotation.z()<<std::endl;


        //Pipe rootPosition
        _val >> z;
        _val >> y;
        _val >> x;

        rootPosition = osg::Vec3(x, y, z);

        //std::cout<<"rootPosition: "<<rootPosition[0]<<";"<<rootPosition[1]<<";"<<rootPosition[2]<<std::endl;
        //for(auto i : rootRotation) std::cout<<"rootRotation: "<<rootRotation[0]<<";"<<rootRotation[1]<<";"<<rootRotation[2]<<std::endl;
        //for(int i=0; i < rotations.size(); i++) std::cout<<"Rotation "<<i<<": "<<rotations[i].x()<<";"<<rotations[i].y()<<";"<<rotations[i].z()<<";"<<rotations[i].w()<<std::endl;
        //std::cout<<"------------"<<std::endl;
        vh->setPosture(rotationBoneNamesVec, rotations, rotationTimes);

        //Fix needed,  setPosition -> setRootTransformation
        //vh->setPosition(positions[0],translationTimes[0]);

        vh->setRootTransformation(rootRotation, rootPosition,  playbacktime );
    }
}

#endif

void CaveAndLocalWrapper::fixReceivedRobotRotations(std::vector<osg::Quat> &rotations, std::vector<std::string> &rotationBoneNamesVec, int &index){
    if (rotationBoneNamesVec[index].compare("r_shoulder") == 0 || rotationBoneNamesVec[index].compare("l_shoulder") == 0) {
        rotations[index] = osg::Quat(osg::PI, osg::Y_AXIS) * rotations[index] * osg::Quat(-osg::PI, osg::Y_AXIS);

    } else if (rotationBoneNamesVec[index].compare("r_forearm") == 0 ||
               rotationBoneNamesVec[index].compare("l_forearm") == 0 ||
               rotationBoneNamesVec[index].compare("r_hand") == 0 ||
               rotationBoneNamesVec[index].compare("l_hand") == 0)
    {
        rotations[index] = osg::Quat(-osg::PI_2, osg::Y_AXIS) *
                osg::Quat(-osg::PI_2, osg::X_AXIS) * rotations[index] *
                osg::Quat( osg::PI_2, osg::X_AXIS) *
                osg::Quat(osg::PI_2, osg::Y_AXIS);
    } else if (rotationBoneNamesVec[index].compare("r_hip") == 0 || rotationBoneNamesVec[index].compare("l_hip") == 0) {
        rotations[index] = osg::Quat(osg::PI_2, osg::X_AXIS) *
                osg::Quat(osg::PI_2, osg::Z_AXIS) * rotations[index] *
                osg::Quat(-osg::PI_2, osg::Z_AXIS) *
                osg::Quat(-osg::PI_2, osg::X_AXIS);
    } else if (rotationBoneNamesVec[index].compare("r_thigh") == 0 || rotationBoneNamesVec[index].compare("l_thigh") == 0) {
        rotations[index] = osg::Quat(osg::PI_2, osg::X_AXIS) *
                osg::Quat(osg::PI_2, osg::Y_AXIS) * rotations[index] *
                osg::Quat(-osg::PI_2, osg::Y_AXIS) *
                osg::Quat(-osg::PI_2, osg::X_AXIS);

    } else if (rotationBoneNamesVec[index].compare("r_calf") == 0 || rotationBoneNamesVec[index].compare("l_calf") == 0) {
        rotations[index] = osg::Quat(osg::PI_2, osg::X_AXIS) *
                osg::Quat(osg::PI_2, osg::Y_AXIS) * rotations[index] *
                osg::Quat(-osg::PI_2, osg::Y_AXIS) *
                osg::Quat(-osg::PI_2, osg::X_AXIS);

    } else if (rotationBoneNamesVec[index].compare("r_ankle") == 0 || rotationBoneNamesVec[index].compare("l_ankle") == 0) {
        rotations[index] = osg::Quat(-osg::PI_2, osg::Z_AXIS) *
                osg::Quat(-osg::PI_2, osg::Z_AXIS) * rotations[index] *
                osg::Quat( osg::PI_2, osg::Z_AXIS) *
                osg::Quat( osg::PI_2, osg::Z_AXIS);


    }else if (rotationBoneNamesVec[index].compare("r_hindfoot") == 0 || rotationBoneNamesVec[index].compare("l_hindfoot") == 0) {

//        iMod::Configuration::getInstance().startCyclicRead();
        rotations[index] = osg::Quat(-osg::PI_2, osg::X_AXIS) *
                osg::Quat(-osg::PI_2, osg::Z_AXIS) * rotations[index] *
                osg::Quat( osg::PI_2, osg::Z_AXIS) *
                osg::Quat( osg::PI_2, osg::X_AXIS);
    }
}

void CaveAndLocalWrapper::updatePosture(VirtualHuman *vhuman, iMod::BlenderController *blend, std::vector<std::string> positionBoneNamesVec, std::vector<std::string> rotationBoneNamesVec, std::string threadName, double playbacktime )
{

    //! The new posture
    std::vector<osg::Quat> rotations;
    std::vector<double> rotationTimes;

    //! New root transformation
    osg::Vec3 rootPosition;
    osg::Quat rootRotation;

    //! Reserve memory
    rotations.resize(rotationBoneNamesVec.size());
    rotationTimes.resize(rotationBoneNamesVec.size());

    //! Reserve memory for a posture and initialize with zero
    iMod::Pose_t posture(rotationBoneNamesVec.size() * 4 + 3 + 4, 0);

    //! If there are no postures skip
    if (!blend->poseContainer->isContainerFilled())
        return;

    //! Get the latest received posture
    posture = blend->poseContainer->getLastPose();

    //!Check if the posture we got is new or still the old one
    //bool is_equal = std::equal(posture.begin(), posture.end(), oldPose.begin());
    //if (is_equal)
    //    continue;

    //! Set the new position
    rootPosition = osg::Vec3(posture[0], posture[1], posture[2]);

    //! Set the new rotation
    rootRotation = osg::Quat(posture[3], posture[4], posture[5], posture[6]);

    //! Set and fix received rotations
    int index = 7; //! The first 3 values in the posture are a position x,y,z of the root bone

    for (int k = 0; k < rotationBoneNamesVec.size(); k++) {
        double x,y,z,w;
        x=posture[index]; ++index;
        y=posture[index]; ++index;
        z=posture[index]; ++index;
        w=posture[index];

        rotations[k] = osg::Quat(x,y,z,w);
        rotationTimes[k] = playbacktime;

        //If we're currently updating a virtual humans posture then fix some rotations, skip if otherwise
        std::size_t found = threadName.find("NAO_");
        if(found != std::string::npos)
            fixReceivedRobotRotations(rotations, rotationBoneNamesVec, k);

        ++index;
    }

    //! Creating xs::Event for joint positions as well as rotations
#ifdef IMOD_CAVE
    xs::Binary _val;

    //osg::Vec3 rootPosition
    _val << (double)rootPosition.x();
    _val << (double)rootPosition.y();
    _val << (double)rootPosition.z();

    //osg::Quat rootRotation
    //std::cout<<"RootRotOld: "<<rootRotation.w()<<";"<<rootRotation.x()<<";"<<rootRotation.y()<<";"<<rootRotation.z()<<std::endl;
    _val << rootRotation.x();
    _val << rootRotation.y();
    _val << rootRotation.z();
    _val << rootRotation.w();


    //double playbacktime
    _val << playbacktime;

    //Pipe rotations
    for(int i=0;i<rotations.size();i++){
        _val << rotations[i].x();
        _val << rotations[i].y();
        _val << rotations[i].z();
        _val << rotations[i].w();
        //std::cout<<"OldRot"<<i<<": "<<rotations[i].x()<<";"<<rotations[i].y()<<";"<<rotations[i].z()<<";"<<rotations[i].w()<<std::endl;
//         std::cout<<"rotationOld:"<<rotations[i].x()<<std::endl;
//         std::cout<<"rotationOld:"<<rotations[i].y()<<std::endl;
//         std::cout<<"rotationOld:"<<rotations[i].z()<<std::endl;
//         std::cout<<"rotationOld:"<<rotations[i].w()<<std::endl;
    }

    //Pipe rotations timings
    for(int i=0;i<rotationTimes.size();i++){
        _val << rotationTimes[i];
    }

    xs::Event ev(xs::Event::CUSTOM_TYPE, _val);
    ev.setEventName(threadName);
    raiseEvent(ev);

#else
	vhuman->setPosture(rotationBoneNamesVec, rotations, rotationTimes);
    vhuman->setRootTransformation(rootRotation, rootPosition, playbacktime);


#endif

    //update region of interest visualization for the virtual human
//    std::size_t found = threadName.find("VH_1");
//        if(found != std::string::npos)
//            _roiRegion->updateAppearance(rootPosition);

}
