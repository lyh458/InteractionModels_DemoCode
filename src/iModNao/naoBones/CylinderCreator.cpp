#include "CylinderCreator.h"

CylinderCreator::CylinderCreator()
{
       _container.clear();

}

CylinderCreator::~CylinderCreator()
{
    _container.clear();
}


// creates a line-geode between 2 bones and adds it to the container
osg::Geode* CylinderCreator::addGeode(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius)
{
    osg::Vec3 startVec = start->getWorldMatrices()[0].getTrans();
    osg::Vec3 endVec = end->getWorldMatrices()[0].getTrans();

    osg::Vec4 color = getColorForLine(startVec, endVec);
    osg::Geode* geode = createCylinderGeode(startVec, endVec, radius, color);

    _container.insert(make_pair(make_pair(start, end), geode));

    return geode;
}

// returns a the line-geode between 2 bones
osg::Geode* CylinderCreator::getGeode(osgAnimation::Bone* start, osgAnimation::Bone* end)
{
     return _container.at(make_pair(start, end));
}

// creates a geode which begins at start vertice and ends at end vertice
osg::Geode* CylinderCreator::createCylinderGeode(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color)
{
    osg::Geode* geode    = new osg::Geode();
    osg::Geometry* geometry = createGeometry(start, end, radius, color);

    geode->addDrawable(geometry);
    geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(start, radius)));
    geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(end, radius)));

    return geode;
}

osg::Geometry* CylinderCreator::createGeometry(osg::Vec3 start, osg::Vec3 end, float radius, osg::Vec4 color)
{
    osg::Geometry* geometry= new osg::Geometry();
    osg::Vec3 center;
    float height;

    osg::Cylinder* cylinder;
    osg::ShapeDrawable* cylinderDrawable;

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

void CylinderCreator::updateCylinderBetweenBones(osgAnimation::Bone* start, osgAnimation::Bone* end, float radius, osg::ref_ptr<osg::Group> inRootNode)
{
    if (!isGeodeInContainerForCylinder(start, end)) {
        osg::Geode* geode = addGeode(start, end, radius);
        inRootNode->addChild(geode);

        osg::notify(osg::DEBUG_INFO)<<"[CylinderCreator] Added Cylinder from startBone:" << start->getName() << " to endBone:" << end->getName() << std::endl;
    } else {            // hier wird geprüft ob es notwendig ist die Cylinder zu updaten
        osg::Geode *geode = getGeode(start, end);
        osg::Vec3 startVec = start->getWorldMatrices()[0].getTrans();
        osg::Vec3 endVec = end->getWorldMatrices()[0].getTrans();

        osg::Cylinder* cylinder = (osg::Cylinder*)(geode->getDrawable(0));
        osg::Vec3f cylinderCenter = (cylinder)->getCenter();
        if (cylinderCenter != ((startVec+endVec)/2)) // Nur wenn sich etwas geändert hat die Cylinder updaten
        {
            geode->removeDrawables(0);
            geode->removeDrawables(1);
            geode->removeDrawables(2);
            geode->addDrawable((osg::Drawable*) createGeometry(startVec, endVec, radius));
            geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(startVec, radius)));
            geode->addDrawable(new osg::ShapeDrawable(new osg::Sphere(endVec, radius)));
            osg::notify(osg::DEBUG_INFO) << "[CylinderCreator] Updated a Cylinder between startBone: " << start->getName() << " and endBone: " << end->getName() << std::endl;
        }
    }
}
