/*
 * LineCreator.h
 *
 *  Created on: Nov 17, 2013
 *      Author: Ben Lorenz, Christian Seidel
 */

#ifndef LINE_CREATOR_H
#define LINE_CREATOR_H

#include <osgAnimation/Bone>
#include <osg/Geode>
#include <osg/Group>
#include <osg/Geometry>
#include <osg/Drawable>
#include <osg/Point>
#include <osg/LineWidth>

#include <map>
#include <iostream>
#include <stdexcept>


using namespace std;

enum LineCreatorColoring {
    // all lines are black
    NOCOLOR=0,
    // all lines are red
    REDCOLOR=1,
    // all lines are white
    WHITECOLOR=2,
    // fixed scale which will be mapped to the spectrum of light
    FIXEDSCALE=3,
    // lines will be colored according to their change in length (lines getting longer = green, lines getting shorter = red)
    CHANGES=4,
    // lines will be random colored
    DISCO=5,
    // lines will be colored according to their change in length in a smooth transition (long lines red, short green)
    // initally all lines are black, because there are no changes
    CHANGESBEN=6
};

class LineCreator {
    // settings for line coloring
    LineCreatorColoring _colorSetting;
    // LineContainer - holds the lines between 2 bones
    std::map<pair<osgAnimation::Bone*,osgAnimation::Bone*> , osg::Geode*> _container;
    
public:
    // contructor
    LineCreator(LineCreatorColoring coloringMode = LineCreatorColoring::REDCOLOR);
    
    // deconstructor
	virtual ~LineCreator();
    
    // adds / update a line between start bone and end bone in osg group inRootNode
    void updateLineBetweenBones(osgAnimation::Bone* start, osgAnimation::Bone* end, osg::ref_ptr<osg::Group> inRootNode);

    // finds a recursivly a node, specified in searchname under the currNode node
    osg::Node* findNamedNode(const std::string& searchName, osg::Node* currNode);

private:
    double getDistance(osg::Vec3 start, osg::Vec3 end);
    double getDistance(osgAnimation::Bone *startBone, osgAnimation::Bone* endBone);
    
    // creates a line-geode between 2 bones and adds it to the container
    osg::Geode* addGeode(osgAnimation::Bone* start, osgAnimation::Bone* end);
    
    //removes a specific line-geode from the container
    void removeGeode(osgAnimation::Bone* start, osgAnimation::Bone* end);
    
    // returns a the line-geode between 2 bones
    osg::Geode* getGeode(osgAnimation::Bone* start, osgAnimation::Bone* end);
    
    // checks if a line exists between 2 bones
    bool isGeodeInContainerForLine(osgAnimation::Bone* start, osgAnimation::Bone* end);
    
    // creates a line-geometry to connect to bones
    osg::Geometry* createGeometry(osg::Vec3 start, osg::Vec3 end, osg::Vec4 color = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    // creates a geode which begins at start vertice and ends at end vertice
    osg::Geode* createLineGeode(osg::Vec3 start, osg::Vec3 end, osg::Vec4 color = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    // logic for calculating a color for a line, based on start and end vertice
    osg::Vec4 getColorForLine(osg::Vec3 startVec, osg::Vec3 endVec);
    // and based on changes between old and new start and end vertice
    osg::Vec4 getColorForLine(osg::Vec3 startVec, osg::Vec3 endVec, osg::Vec3 oldStartVec, osg::Vec3 oldEndVec, osg::Vec4 oldColor = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f));
    
    // converts a wavelength info the RGB colorspace
    // implementation from: http://www.efg2.com/Lab/ScienceAndEngineering/Spectra.htm
    osg::Vec4 wavelengthToRGB(int wavelength);
    
    // adjusts a color by a given factor (0..1), gamma and maximum intensity
    osg::Vec4 adjustColor(osg::Vec4 color, float factor, float gamma, float intensityMax);
};
#endif
