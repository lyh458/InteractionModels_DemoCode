#include "CBandNaoRobotCreator.h"


CBandNaoRobotCreator::CBandNaoRobotCreator()
{
    //ctor
}

CBandNaoRobotCreator::~CBandNaoRobotCreator()
{
    //dtor
}
map<string,osgAnimation::Bone*> buildBoneMap(osg::Node* node)
{
	BoneFinder bf;
	bf.apply(*node);
	map<string,osgAnimation::Bone*> bonemap = bf.getNodeMap();
	return bonemap;
}


osg::ref_ptr<osg::Node> CBandNaoRobotCreator::initCBorNaoMeshes(osg::ref_ptr<osg::Switch> rootSwitch)

{

	//						    / ---tubeRoot(osg::Group)
	//rootSwitch---<
	//						    \ ---meshHuman(osg::Node)

		osg::ref_ptr<osg::Group> tubeRoot = new osg::Group();
		//osg::ref_ptr<osg::Group> meshRoot = new osg::Group();
 		//generate tubes
			CylinderCreator* cc = new CylinderCreator();
			BoneGen* bg = new BoneGen();
			osg::ref_ptr<osg::Node> vhuman = osgDB::readNodeFile("Nao.osgt");
			if (!vhuman)
			{
				std::cout <<"Nao.osgt" << std::endl;
				return NULL;
			}
			tubeRoot->addChild(vhuman);

			// find the bones of the virtual human
			map<string,osgAnimation::Bone*> bonemap = buildBoneMap(vhuman);

			//generate bonepair map with the form (bone* child, bone* parent)
			map<osgAnimation::Bone*, osgAnimation::Bone*> bp = bg->genBonesFromBonemap(bonemap, vhuman, tubeRoot, cc);

			bg->init(bp, cc, tubeRoot);

		//load mesh
			osg::ref_ptr<osg::Node> meshHuman = osgDB::readNodeFile("NaoRigShape.osgt");
			if (!meshHuman)
			{
				std::cout <<"Could not load NaoRigShape.osgt" << std::endl;
				return NULL;
			}
			
			rootSwitch->addChild(meshHuman);
			rootSwitch->addChild(tubeRoot);
			//rootSwitch->addChild(meshRoot);
			rootSwitch->setEventCallback(new SwitchNaoKeyboardEventHandler);

		//return (osg::ref_ptr<osg::Node>)rootSwitch;
		return tubeRoot;
}
