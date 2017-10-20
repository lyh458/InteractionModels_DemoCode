#include "CylinderCreator.h"

CylinderCreator::CylinderCreator()
{
       _container.clear();

}

CylinderCreator::~CylinderCreator()
{
    _container.clear();
}


// creates a cylinder-geode between 2 bones(start = childBone, end = parentBone) and adds it to the container
osg::Geode* CylinderCreator::addGeode(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius)
{

	osg::Geode* geode;
	osg::Vec3 startVec;
	osg::Vec3 endVec;
	endVec = osg::Vec3(0, 0, 0);							//parent
	if((start->getName() != "r_shoulder" )&&   (start->getName() != "l_shoulder" ))
	{
		startVec = getBoneStackedTrans(start);
	}
	else{
		//cause default shoulder's parent is Torso, but tubes generated not perfect
		//this methode create tubes between shoulder and neckYaw

		//get neckYaw coodinate
		osg::Vec3 neckYawVec = getBoneStackedTrans((osgAnimation::Bone*)((osgAnimation::Bone*)(start->getParent(0)))->getChild(0));

		//child r_shoulder l_shoulder local coordinaten
		startVec = getBoneStackedTrans(start);

		//neckYaw coordinate
		endVec = neckYawVec;
	}

	geode = createCylinderGeode(startVec, endVec, radius);
	_container.insert(make_pair(make_pair(start, end), geode));

    return geode;
}

// returns a the cylinder-geode between 2 bones
osg::Geode* CylinderCreator::getGeode(osgAnimation::Bone* start, osgAnimation::Bone* end)
{
     return _container.at(make_pair(start, end));
}

// creates a cylinder geode which begins at start vertice and ends at end vertice
osg::Geode* CylinderCreator::createCylinderGeode(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color)
{
    osg::Geode* geode    = new osg::Geode();
    osg::Geometry* geometry = createGeometry(start, end, radius, color);

	//add geometry to geode
    geode->addDrawable(geometry);
	osg::ShapeDrawable* startSphere = new osg::ShapeDrawable(new osg::Sphere(start, radius));
	startSphere->setColor(color);

	osg::ShapeDrawable* endSphere = new osg::ShapeDrawable(new osg::Sphere(end, radius));
	endSphere->setColor(color);
	geode->addDrawable(startSphere);
	geode->addDrawable(endSphere);

    return geode;
}

//create cylinder geometry
osg::Geometry* CylinderCreator::createGeometry(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color)
{
    osg::Geometry* geometry= new osg::Geometry();
    osg::Vec3 center;
    float height;

    osg::Cylinder* cylinder;
    osg::ShapeDrawable* cylinderDrawable;

	//height and center coordinate of the cylinder
    height = (start-end).length();
    center = osg::Vec3( (start.x() + end.x())/2, (start.y() + end.y())/2, (start.z() + end.z())/2 );

    //default direction for cylinders to face
    osg::Vec3 z = osg::Vec3(0, 0, 1);

    //get diff between two points
    osg::Vec3 p = start-end;

    //get CROSS product (the axis of rotation)
    osg::Vec3 t = z ^ p;

    //get angle. length is magnitude of the vector
    double angle = acos((z*p)/p.length());

    //creat cylinder between the two points
    cylinder = new osg::Cylinder(center, radius, height);
    cylinder->setRotation(osg::Quat(angle, osg::Vec3(t.x(), t.y(), t.z())));
    cylinderDrawable = new osg::ShapeDrawable(cylinder);
	cylinderDrawable->setColor(color);

    geometry = (osg::Geometry*)cylinderDrawable;
    return  geometry;
}

bool CylinderCreator::isGeodeInContainerForCylinder(osgAnimation::Bone* start, osgAnimation::Bone* end) {
        try {
            getGeode(start, end);
        }
        catch (std::out_of_range error) {
            return false;
        }

        return true;
}

void CylinderCreator::updateCylinderBetweenBones(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius, osg::ref_ptr<osg::Group> inRootNode, osg::Vec4 color)
{
	//if the geode not existiert then create a new one
    if (!isGeodeInContainerForCylinder(start, end)) {
		osg::Geode* geode = addGeode(start, end, radius);
		end->addChild(geode);

        osg::notify(osg::DEBUG_INFO)<<"[CylinderCreator] Added Cylinder from startBone:" << start->getName() << " to endBone:" << end->getName() << std::endl;
	}
	else 	{
		// geode existiert and necessary to update
		osg::Geode *geode = getGeode(start, end);

		osg::Vec3 startVec = getBoneStackedTrans(start);
		osg::Vec3 endVec = osg::Vec3(0,0,0);

        osg::Cylinder* cylinder = (osg::Cylinder*)(geode->getDrawable(0));
        osg::Vec3f cylinderCenter = (cylinder)->getCenter();
        if (cylinderCenter != ((startVec+endVec)/2)) // Nur wenn sich etwas geändert hat die Cylinder updaten
        {
            geode->removeDrawables(0);
            geode->removeDrawables(1);
            geode->removeDrawables(2);
			geode->addDrawable((osg::Drawable*) createGeometry(startVec, endVec, radius, color));
            geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(startVec, radius)));
            geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(endVec, radius)));
            osg::notify(osg::DEBUG_INFO) << "[CylinderCreator] Updated a Cylinder between startBone: " << start->getName() << " and endBone: " << end->getName() << std::endl;
        }
    }
}

osg::Vec3 CylinderCreator::getBoneStackedTrans(osgAnimation::Bone* bone)
{
	osgAnimation::UpdateBone* loadedUpdateStartBone = (osgAnimation::UpdateBone*) bone->getUpdateCallback();
	loadedUpdateStartBone ->getStackedTransforms().update();
	return loadedUpdateStartBone ->getStackedTransforms().getMatrix().getTrans();
}
