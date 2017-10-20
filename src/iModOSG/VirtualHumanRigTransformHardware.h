/*!
 * VirtualHumanRigTransformHardware.h
 *
 *  Created on: Jan 08, 2014
 *     Authors: dmueller
 */

#ifndef RIGHW_H
#define RIGHW_H 1

#include <iostream>
#include <sstream>
#include <osgAnimation/RigGeometry>
#include <osgAnimation/RigTransformHardware>
#include <osgAnimation/RigTransformSoftware>
#include <osgAnimation/BoneMapVisitor>
#include <osg/Program>
#include <osg/Material>

/*!
 * @brief VirtualHumanRigTransformHardware will use software rendering for drawables with less vertices than this number
 */
#define __VHRTH_FORCE_SOFTWARE_VERTICE_COUNT__ 10000

/*!
 * @brief VirtualHumanRigTransformHardware class
 * @details This class can be used to set either hardware of software rendering for skeleton based avatars.
 */
class VirtualHumanRigTransformHardware : public osgAnimation::RigTransformHardware
{
	public:
		/**
		 * @brief Enumeration used to identify rendering mode
		 * @description The rendering mode can either be always hardware, hardware if enough vertices have been found, or always software rendering
		 */
		enum HardwareRenderingMode
		{
		    /** Render everything hardware based **/
		    ON=0,
		    /** Render object based on number of vertices (use __VHRTH_FORCE_SOFTWARE_VERTICE_COUNT__ class constant) **/
		    POSSIBLE=1,
		    /** Don't apply hardware rendering **/
		    OFF=2
		};
		/**
		 * @brief Enumeration used to identify coloring mode
		 * @description The color mode will influence material properties for hardware rendered objects
		 */
		enum HardwareRenderingColorMode
		{
		    /** Don't apply additional Material Changes **/
		    STANDARD=0, 
		    /** Make it brighter, apply Specular Light **/
		    BRIGHT=1, 
		    /** .. **/
		    NINJA=2, 
		    /** ?? **/
		    SICK=3 
		};

		/**
		 * @brief Constructor
		 * @details Sets standard color and rendering modes
		 */ 
		VirtualHumanRigTransformHardware() :
			osgAnimation::RigTransformHardware(), 
			_ColorMode(HardwareRenderingColorMode::STANDARD),
			_RenderingMode(HardwareRenderingMode::POSSIBLE) { };

	    void operator()(osgAnimation::RigGeometry& geom);
	    bool init(osgAnimation::RigGeometry& geom);
	    bool createPalette(int nbVertexes, osgAnimation::BoneMap boneMap, const osgAnimation::VertexInfluenceSet::VertexIndexToBoneWeightMap& vertexIndexToBoneWeightMap);

	    /**
	     * @brief Set the mode to one of the HardwareRenderingMode enums for hardware or software rendering.
	     * @param mode HardwareRenderingMode Enum
	     */
	    inline void setRenderingMode(HardwareRenderingMode mode) {  _RenderingMode = mode; _needInit = true; }

	    /**
		 * @brief Set the mode to one of the HardwareRenderingColorMode enums for lighter / darker material.
	     * @param mode HardwareRenderingColorMode Enum
		 */
	    inline void setMaterialMode(HardwareRenderingColorMode mode) { _ColorMode = mode; }

	private:
		HardwareRenderingColorMode _ColorMode;	    
		HardwareRenderingMode _RenderingMode;	    
};

#endif
