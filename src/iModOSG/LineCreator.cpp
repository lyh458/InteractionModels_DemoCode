#include "LineCreator.h"

LineCreator::LineCreator(LineCreatorColoring coloringMode) {
    _container.clear();
    _colorSetting = coloringMode;
}

LineCreator::~LineCreator() {
	_container.clear();
}

osg::Geode* LineCreator::addGeode(osgAnimation::Bone* start, osgAnimation::Bone* end) {
    osg::Vec3 startVec = start->getWorldMatrices()[0].getTrans();
    osg::Vec3 endVec = end->getWorldMatrices()[0].getTrans();

    osg::Vec4 color = getColorForLine(startVec, endVec);
    osg::Geode* geode = createLineGeode(startVec, endVec, color);
    
    _container.insert(make_pair(make_pair(start, end), geode));
    
    return geode;
}
void LineCreator::removeGeode(osgAnimation::Bone* start, osgAnimation::Bone* end) {
    _container.erase(make_pair(start, end));
}
osg::Geode* LineCreator::getGeode(osgAnimation::Bone* start, osgAnimation::Bone* end) {
    return _container.at(make_pair(start, end));
}
bool LineCreator::isGeodeInContainerForLine(osgAnimation::Bone* start, osgAnimation::Bone* end) {
        try {
            getGeode(start, end);
        }
        catch (std::out_of_range error) {
            return false;
        }

        return true;
}

void LineCreator::updateLineBetweenBones(osgAnimation::Bone* start, osgAnimation::Bone* end, osg::ref_ptr<osg::Group> inRootNode) {
    if (!isGeodeInContainerForLine(start, end)) {
        osg::Geode* geode = addGeode(start, end);
        inRootNode->addChild(geode);
        osg::notify(osg::NotifySeverity::DEBUG_INFO)<<"[LineCreator] Added Line from startBone:" << start->getName() << " to endBone:" << end->getName() << std::endl;
    } else {            // hier wird geprüft ob es notwendig ist die Linie zu updaten
        osg::Geode *geode = getGeode(start, end);
        osg::Vec3 startVec = start->getWorldMatrices()[0].getTrans();
        osg::Vec3 endVec = end->getWorldMatrices()[0].getTrans();
        
        osg::Geometry *geometry = dynamic_cast<osg::Geometry*>(geode->getDrawable(0));
        
        osg::Array *vecArray = geometry->getVertexArray();
        osg::Vec3Array *vec3Array = dynamic_cast<osg::Vec3Array*>(vecArray);
        
        osg::Vec3 oldStartVec = (*vec3Array)[0];
        osg::Vec3 oldEndVec = (*vec3Array)[1];
        
        if (oldStartVec != startVec || oldEndVec != endVec) // Nur wenn sich etwas geändert hat die Linien updaten
        {
            osg::Vec4 oldColor;
            osg::Geometry* oldGeo = (osg::Geometry*) geode->getDrawable(0);
            osg::Vec4Array *oldColorArray = (osg::Vec4Array*) oldGeo->getColorArray();
            oldColor = (*oldColorArray)[0];
            
            osg::Vec4 color = getColorForLine(startVec, endVec, oldStartVec, oldEndVec, oldColor);
            geode->removeDrawables(0);
            geode->addDrawable((osg::Drawable*) createGeometry(startVec, endVec, color));
            osg::notify(osg::NotifySeverity::DEBUG_INFO) << "[LineCreator] Updated a Line between startBone: " << start->getName() << " and endBone: " << end->getName() << std::endl;
        }
    }
}

double LineCreator::getDistance(osgAnimation::Bone *startBone, osgAnimation::Bone* endBone) {
    osg::Vec3 start = startBone->getWorldMatrices()[0].getTrans();
    osg::Vec3 end = endBone->getWorldMatrices()[0].getTrans();
    return (end-start).length();
}

double LineCreator::getDistance(osg::Vec3 start, osg::Vec3 end) {
    return (end-start).length();
}


osg::Geometry* LineCreator::createGeometry(osg::Vec3 start, osg::Vec3 end, osg::Vec4 color) {
    osg::Geometry*  geometry = new osg::Geometry();
    osg::Vec3Array* vertices = new osg::Vec3Array();
    osg::Vec4Array* colors   = new osg::Vec4Array();
    
    vertices->push_back(start);
    vertices->push_back(end);
    vertices->dirty();
    
    
    colors->push_back(color);
    
    colors->dirty();
    
    geometry->setVertexArray(vertices);
    
	
#ifdef OSG_VERSION_LESS_THAN_3_DOT_2
	geometry->setColorArray(colors); // bis OSG 3.1
	geometry->setColorBinding (osg::Geometry::BIND_PER_VERTEX); // bis OSG 3.1
#else
	geometry->setColorArray(colors, osg::Array::BIND_PER_PRIMITIVE_SET); // erst ab OSG 3.2
#endif
	
    geometry->addPrimitiveSet(new osg::DrawArrays(osg::PrimitiveSet::LINES, 0, 2));
    geometry->getOrCreateStateSet()->setMode(GL_LIGHTING, false);
	
	
	osg::LineWidth* lineWidth = new osg::LineWidth();
	lineWidth->setWidth(3.0f);
	geometry->getOrCreateStateSet()->setAttributeAndModes(lineWidth, osg::StateAttribute::ON);
	
    
    geometry->dirtyDisplayList();
    geometry->setUseDisplayList(false);
    geometry->setDataVariance(osg::Object::DYNAMIC);
    
    return geometry;
}

osg::Geode* LineCreator::createLineGeode(osg::Vec3 start, osg::Vec3 end, osg::Vec4 color) {
    osg::Geode*     geode    = new osg::Geode();

    osg::Geometry *geometry = createGeometry(start, end, color);

    geode->addDrawable(geometry);
    
    return geode;
}
osg::Node* LineCreator::findNamedNode(const std::string& searchName, osg::Node* currNode)
{
   osg::Group* currGroup;
   osg::Node* foundNode;

   if ( !currNode)
   {
      return NULL;
   }

   if (currNode->getName() == searchName)
   {
      return currNode;
   }

   currGroup = currNode->asGroup();
   if ( currGroup )
   {
      for (unsigned int i = 0 ; i < currGroup->getNumChildren(); i ++)
      {
         foundNode = findNamedNode(searchName, currGroup->getChild(i));
         if (foundNode)
            return foundNode;
      }
      return NULL;
   }
   else
   {
      return NULL;
   }
}


osg::Vec4 LineCreator::getColorForLine(osg::Vec3 startVec, osg::Vec3 endVec, osg::Vec3 oldStartVec, osg::Vec3 oldEndVec, osg::Vec4 oldColor) {
    osg::Vec4 color;
    
    switch (_colorSetting) {
        case LineCreatorColoring::NOCOLOR:      // all lines black
            color = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            break;
        case LineCreatorColoring::REDCOLOR:     // all lines red
            color = osg::Vec4(1.0f, 0.0f, 0.0f, 0.0f);
            break;
        case LineCreatorColoring::WHITECOLOR:   // all lines white
            color = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            break;
        case LineCreatorColoring::FIXEDSCALE: {    // calculate color based on a min and max distance (idea behind: RGB colors as a function of wavelength for visible light (380 to 780 nm)
            osg::Vec3 pq = endVec-startVec;
            float distance = pq.length();
            
            float minVisibleWaveLength = 380.0f; // 350
            float maxVisibleWaveLength = 780.0f; // 650
            
            float baseDistance = 175;       // 100 + 75 = base distance between characters
            float minDistance = 100;
            float maxDistance = baseDistance+minDistance;
            
            float distanceAsWavelength =
            (distance - minDistance) / (maxDistance-minDistance) * (maxVisibleWaveLength - minVisibleWaveLength) + minVisibleWaveLength;
            
            color = wavelengthToRGB(distanceAsWavelength);
            osg::notify(osg::NotifySeverity::DEBUG_INFO) << "[LineCreator] Set Color of line to r: " << color[0] << " b: " << color[1] << " g: " << color[2] << endl;
        }
            break;
        case LineCreatorColoring::CHANGES: {    // calculate color based on changes between old and a new line
            osg::Vec3 pq = endVec-startVec;
            float distance = pq.length();
            
            if (startVec == oldStartVec && endVec == oldEndVec) { // no old vertices given or no changes
                color = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f);
            } else {
                float distanceOld = (oldEndVec-oldStartVec).length();
                osg::notify(osg::NotifySeverity::DEBUG_INFO) << "[LineCreator] dist: " << distance << " distOld: " << distanceOld << endl;
                if (distance < distanceOld) {
                    color = osg::Vec4(0.0f, 1.0f, 0.0f, 0.0f);  // green if they come closer together
                } else if (distance > distanceOld) {
                    color = osg::Vec4(0.0f, 0.0f, 1.0f, 0.0f); // if they move farther away
                } else {
                    color = osg::Vec4(1.0f, 1.0f, 1.0f, 1.0f); // white if they got exact same distance
                }
            }
        }
            break;
        case LineCreatorColoring::DISCO: {      // calculate color based on random numbers
            int red = rand() % 255;
            int green = rand() % 255;
            int blue = rand() % 255;
            //int alpha = rand() % 256;
            
            float r = ((float)red) / 255;
            float g = ((float)green) / 255;
            float b = ((float)blue) / 255;
            osg::notify(osg::NotifySeverity::DEBUG_INFO) << "[LineCreator] r: " << r << " b: " << g << " g: " << b << endl;
            color = osg::Vec4(r, g, b, 1.0f);
        }
            break;
        case LineCreatorColoring::CHANGESBEN: {
            osg::Vec3 pq = endVec-startVec;
            float distance = pq.length();
            double r = 0;
            double g = 0;
            
            if (startVec == oldStartVec && endVec == oldEndVec) { // no old vertices given or no changes
                color = osg::Vec4(1.0f, 0.0f, 0.0f, 1.0f);
            } else {
                osg::Vec3 p2q2 = oldEndVec-oldStartVec;
                float oldDistance = p2q2.length();
                r = oldColor.r();
                g = oldColor.g();
                
                if (distance < oldDistance) {
                    if(r>=0.05) r = r-0.05;
                    if(g<=0.95) g = g+0.05;
                } else {
                    if(r<=0.95) r = r+0.05;
                    if(g>=0.05) g = g-0.05;
                }
            }
            color = osg::Vec4(r,g,0,1);
        }
            break;
        default:
            color = osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
            break;
    }
    
    return color;
}

osg::Vec4 LineCreator::getColorForLine(osg::Vec3 startVec, osg::Vec3 endVec) {
    return getColorForLine(startVec, endVec, startVec, endVec);
}


osg::Vec4 LineCreator::wavelengthToRGB(int wavelength) {
    float gamma = 0.8f;
    float intensityMax = 255.0f;
    float r,g,b,factor;
    
    switch (wavelength<=439?0 : wavelength<=489?1 : wavelength<=509?2 : wavelength<=579?3 : wavelength<=644?4 : wavelength<=780?5: wavelength) {
        case 0:
            r = -(wavelength - 440.0f) / (440.0f-380.0f);
            g = 0.0f;
            b = 1.0f;
            break;
        case 1:
            r = 0.0f;
            g = (wavelength - 440.0f) / (490.0f - 440.0f);
            b = 1.0f;
            break;
        case 2:
            r = 0.0f;
            g = 1.0f;
            b = -(wavelength - 510.0f) / (510.0f - 490.0f);
            break;
        case 3:
            r = (wavelength - 510.0f) / (580.0f - 510.0f);
            g = 1.0f;
            b = 0.0f;
            break;
        case 4:
            r = 1.0f;
            g = -(wavelength - 645.0f) / (645.0f - 580.0f);
            b = 0.0f;
            break;
        case 5:
            r = 1.0f;
            g = 0.0f;
            b = 0.0f;
            break;
            
        default:
            r = 0.0f;
            g = 0.0f;
            b = 0.0f;
            break;
    }
    
    // Let the intensity fall off near the vision limits
    switch (wavelength<=419?0 : wavelength<=700?1 : wavelength<=780?2 : wavelength) {
        case 0:
            factor = 0.3f + 0.7f * (wavelength - 380.0f) / (420.0f - 380.0f);
            break;
        case 1:
            factor = 1.0f;
            break;
        case 2:
            factor = 0.3f + 0.7f * (780.0f - wavelength) / (780.0f - 700.0f);
            break;
        default:
            factor = 0.0f;
            break;
    }
    
    return adjustColor(osg::Vec4(r, g, b, 1.0f), factor, gamma, intensityMax);
}

osg::Vec4 LineCreator::adjustColor(osg::Vec4 color, float factor, float gamma, float intensityMax) {
    osg::Vec4 adjustedColor;
    if (color[0] == 0.0f && color[1]==0.0f && color[2]==0.0f) {
        return osg::Vec4(0.0f, 0.0f, 0.0f, 0.0f);
    } else {
        //roundf(intensityMax * pow(color * factor, gamma));
        adjustedColor[0] = roundf(intensityMax * pow(color[0] * factor, gamma));
        adjustedColor[1] = roundf(intensityMax * pow(color[1] * factor, gamma));
        adjustedColor[2] = roundf(intensityMax * pow(color[2] * factor, gamma));
        adjustedColor[3] = color[3];
    }
    return adjustedColor;
}

