//
//  MyInteractionLineMeshView.h
//  BoneAnimation
//
//  Created by Christian on 30.11.13.
//
//

#ifndef __InteractionROI__
#define __InteractionROI__

#include <osgParticle/Particle>
#include <osgParticle/ParticleSystem>
#include <osgParticle/ParticleSystemUpdater>
#include <osgParticle/ModularEmitter>
#include <osgParticle/RandomRateCounter>
#include <osgParticle/SectorPlacer>
#include <osgParticle/RadialShooter>
#include <osgParticle/ModularProgram>
#include <osgParticle/AccelOperator>
#include <osgParticle/FluidFrictionOperator>

/**
 * @brief A visual region of interest in which a user can interact with the virtual agent.
 * It is a OSG particle system
 */
class InteractionROI {

    osgParticle::ParticleSystem* _ps;
    osgParticle::ModularEmitter* _emitter;
    osgParticle::Particle ptemplate;
    osg::Vec3 _roiCenter;
    double _roiRadius;

public:
	/**
	 * @brief Constructor
	 * 
	 * @param root Pointer to a osg root node
	 */
    InteractionROI();

    /**
     * @brief Deconstructor
     */
    virtual ~InteractionROI();
    
    
    void add(osg::ref_ptr<osg::Group> root);

    /*
    * Toggle the ROIs color based on the user position.
    */
    void updateAppearance(osg::Vec3 userPosition);

private: 

    /*
    * Create a OSG particle system to visualize the region of interest
    */
    void createParticleSystem();
    
    /*
     * Toggle the ROI on or off.
     * If the user is within the ROI it is toggled on @see updateAppearance
     */
    void setROIactive();
    void setROIpassive();

};


#endif /* defined(__InteractionROI__) */
