#include "VirtualHumanNao.h"

int VirtualHumanNao::_childNum =0;


VirtualHumanNao::VirtualHumanNao() :VirtualHuman(){
}

VirtualHumanNao::~VirtualHumanNao() {
	// TODO Auto-generated destructor stub
}

map<string,osgAnimation::Bone*> buildBoneMap(osg::Node* node)
{
	BoneFinder bf;
	bf.apply(*node);
	map<string,osgAnimation::Bone*> bonemap = bf.getNodeMap();
	return bonemap;
}

void VirtualHumanNao::loadNaoTube(){

	osg::ref_ptr<osg::Group> tubeGroup = new osg::Group();

	//generate tubes
		CylinderCreator* cc = new CylinderCreator();
		BoneGen* bg = new BoneGen();
		osg::Node* naoTube = osgDB::readNodeFile("Nao.osgt");
		if (!naoTube)
		{
			std::cout <<"Nao.osgt" << std::endl;
			return;
		}
		tubeGroup->addChild(naoTube);

		// find the bones of the virtual human
		map<string,osgAnimation::Bone*> bonemap = buildBoneMap(naoTube);

		//generate bonepair map with the form (bone* child, bone* parent)
		map<osgAnimation::Bone*, osgAnimation::Bone*> bp = bg->genBonesFromBonemap(bonemap, naoTube, tubeGroup, cc);

		bg->init(bp, cc, tubeGroup);
		_model = (osg::Node*)tubeGroup.get();
}

void VirtualHumanNao::loadNaoMesh(){
	osg::ref_ptr<osg::Group> meshGroup = new osg::Group();
	//load mesh
	osg::Node* meshHuman = osgDB::readNodeFile("NaoRigShape.osgt");
	if (!meshHuman)
	{
		std::cout <<"Could not load NaoRigShape.osgt" << std::endl;
		return;
	}

	meshGroup->addChild(meshHuman);
	_model = (osg::Node*)meshGroup.get();
}