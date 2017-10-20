/*
* "./cylinderToBones Nao.osgt" to load the model 
*
*/

#include <osg/Group>

#include <osgAnimation/Bone>
#include <osgAnimation/Skeleton>

#include <osgDB/Options>
#include <osgDB/ReaderWriter>
#include <osgDB/ReadFile>
#include <osgDB/WriteFile>
#include <osgDB/Registry>
#include <osgGA/StateSetManipulator>
#include <osgGA/TrackballManipulator>


#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>


#include <iostream>
#include <vector>
#include <map>

#include "BoneFinder.h"
#include "SkeletonFinder.h"
#include "BoneGen.h"
#include "CylinderCreator.h"

#include <osg/ApplicationUsage>
using namespace std;

osg::Geode* createAxis(){
	osg::Geode*     geode    = new osg::Geode();
	osg::Geometry*  geometry = new osg::Geometry();
	osg::Vec3Array* vertices = new osg::Vec3Array();
	osg::Vec4Array* colors   = new osg::Vec4Array();

	vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(0.2f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, 0.2f, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.0f));
	vertices->push_back(osg::Vec3(0.0f, 0.0f, 0.2f));

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

map<string,osgAnimation::Bone*> buildBoneMap(osg::Node* node)
{
	BoneFinder bf;
	bf.apply(*node);
	map<string,osgAnimation::Bone*> bonemap = bf.getNodeMap();
	return bonemap;
}

int main(int argc, char**argv)
{
    osg::ArgumentParser arguments(&argc, argv);
	// build scene
	osg::ref_ptr<osg::Group> root = new osg::Group;
    osgViewer::Viewer viewer;
    CylinderCreator* cc = new CylinderCreator();
    BoneGen* bg = new BoneGen();
	//string modelfilename = "Data/HippieGirl/HippieGirl-hanim.osgt";
	//string modelfilename = "Data/JohnnyDepp/JohnnyDepp-hanim.osgt";
	//string modelfilename = "Data/Geek/Geek-hanim.osgt";
	//string modelfilename = "Data/Bricia/bricia-hanim.osgt";

    //string modelfilename = "JohnnyDepp-hanim.osgt";
    //string modelfilename = "Nao.osgt";
	osg::ref_ptr<osg::Node> vh = osgDB::readNodeFiles(arguments);
    osg::Node* vhuman = vh.get();

	if (!vhuman)
    {
        std::cout <<"Could not load the virtual human model" << std::endl;
        return 1;
    }

	root->addChild(vhuman);

	//osg::Node* axes = osgDB::readNodeFile("axes.osgt");
	//m

	// find the bones of the virtual human
	map<string,osgAnimation::Bone*> bonemap = buildBoneMap(vhuman);

	cout << "BONE MAP size: " << bonemap.size() << endl;
	cout << "FETCHING BONES:" << endl;
	map<string,osgAnimation::Bone*>::const_iterator it;
	for(it = bonemap.begin(); it != bonemap.end(); ++it)
	{
        osg::Geode* axes = createAxis();
		osgAnimation::Bone* parent = (*it).second->getBoneParent();
		string parentname = "-";
		if (parent) parentname= parent->getName();
        cout << "   BONE NAME: " << (*it).first << "     PARENT: " << parentname << endl;

		(*it).second->addChild(axes);
	}

    //generate bonepair map with the form (bone* child, bone* parent)
    map<osgAnimation::Bone*, osgAnimation::Bone*> bp = bg->genBonesFromBonemap(bonemap, vhuman, root, cc);
//    map<osgAnimation::Bone*, osgAnimation::Bone*> bp = bg->genBonesFromBonemap(bonemap, vhuman, root, lc);
    std::cout<<"bone pair size: "<<bp.size()<<std::endl;


	// find the skeleton (probably we must never do this, but we know how to do it)
	SkeletonFinder sf;
	sf.apply(*vhuman);
	int numSkeletons = sf.getNodeList().size();
	cout << "NUMBER OF SKELETONS: " << numSkeletons << endl;
	//osgAnimation::Skeleton* skeleton = sf.getFirst();

    viewer.setSceneData(root.get());
	viewer.addEventHandler(new osgViewer::StatsHandler);
	viewer.addEventHandler(new osgViewer::WindowSizeHandler);
    viewer.addEventHandler(new osgViewer::ThreadingHandler);
    viewer.addEventHandler(new osgViewer::HelpHandler);
	viewer.addEventHandler(new osgGA::StateSetManipulator(viewer.getCamera()->getOrCreateStateSet()));
    viewer.setCameraManipulator(new osgGA::TrackballManipulator);
    viewer.realize();


    float radius = 0.2;
    while(!viewer.done()){
        map<osgAnimation::Bone*, osgAnimation::Bone*>::const_iterator ite;
        for(ite = bp.begin(); ite != bp.end(); ++ite)
        {
            if((*ite).first->getBoneParent()->getBoneParent())
            {
                cc->updateCylinderBetweenBones((*ite).first, (*ite).second, radius, root);
            }
        }
//            viewer.run();
//while(!viewer.done()){
        viewer.frame();
    }

}
