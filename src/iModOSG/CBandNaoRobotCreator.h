/*
 * CBandNaoRobotCreator.h
 *
 *  Created on: Feb. 06 2014
 *      Author: Mengzhi Zhang, Hongchang Hao
 */

#ifndef CBANDNAOROBOTCREATOR_H
#define CBANDNAOROBOTCREATOR_H

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
//#include "SkeletonFinder.h"
#include "BoneGen.h"
#include "CylinderCreator.h"
#include "SwitchNaoKeyboardEventHandler.h"
#include "VirtualHumanNao.h"
#include <osg/ApplicationUsage>

class CBandNaoRobotCreator
{
    public:
	CBandNaoRobotCreator();
	virtual ~CBandNaoRobotCreator();

	/**
	 *create the Group(Switch) include childs (Tubes and Mesh Robot)
	 *@param rootSwitch pointer osg::Switch
	 *@return pointer Node include the childs
	 */
	osg::ref_ptr<osg::Node> initCBorNaoMeshes(osg::ref_ptr<osg::Switch> rootSwitch);

};

#endif // CBANDNAOROBOTCREATOR_H
