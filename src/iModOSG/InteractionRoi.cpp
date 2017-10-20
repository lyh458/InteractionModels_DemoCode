#include "InteractionROI.h"

#include <cmath>        // std::abs
#include <iostream>

InteractionROI::InteractionROI() {
	_roiCenter = osg::Vec3(-0.35, 0.72, -1.27);
	_roiRadius = 0.75;
}

InteractionROI::~InteractionROI() {
    
}


void InteractionROI::add(osg::ref_ptr<osg::Group> root){

	createParticleSystem();
	root->addChild(_emitter);

	osg::Geode *geode = new osg::Geode;    
    geode->addDrawable(_ps);

    // add the geode to the scene graph
    root->addChild(geode);

    osgParticle::ParticleSystemUpdater *psu = new osgParticle::ParticleSystemUpdater;
    psu->addParticleSystem(_ps);

	root->addChild(psu);  
}

void InteractionROI::createParticleSystem(){

    ptemplate.setLifeTime(1);
    ptemplate.setRadius(0.05f);    // 5 cm wide particles
    ptemplate.setMass(0.05f);    // 50 g heavy

	_ps = new osgParticle::ParticleSystem;
	_ps->setDefaultAttributes("objects/roiParticleGreen-01.png", true, false);
    //_ps->setDefaultAttributes("", false, false);
    _ps->setDefaultParticleTemplate(ptemplate);
    _ps->setParticleAlignment(osgParticle::ParticleSystem::Alignment::BILLBOARD);

    setROIactive();
    
	_emitter = new osgParticle::ModularEmitter;
	_emitter->setParticleSystem(_ps);

	osgParticle::RandomRateCounter *counter = new osgParticle::RandomRateCounter;
    counter->setRateRange(450, 750);
    _emitter->setCounter(counter);

    osgParticle::SectorPlacer *placer = new osgParticle::SectorPlacer;
    placer->setCenter(_roiCenter[0],_roiCenter[1],_roiCenter[2]);
    placer->setRadiusRange(0.75, 0.5);
    placer->setPhiRange(0, 2 * osg::PI);    // 360∞ angle to make a circle
    _emitter->setPlacer(placer);

    osgParticle::RadialShooter *shooter = new osgParticle::RadialShooter;
    shooter->setInitialSpeedRange(0, 0);
    _emitter->setShooter(shooter);

}

void InteractionROI::updateAppearance(osg::Vec3 userPosition){

	osg::Vec3 absRoiCenter = osg::Vec3((_roiCenter[0]),(_roiCenter[1]),(_roiCenter[2]));
	osg::Vec3 absPos = osg::Vec3((userPosition[0]),(userPosition[1]),(userPosition[2]));
	
	bool insideROI = true;
	for(int i=0;i<2;i++){
		if(std::abs(absRoiCenter[i]-absPos[i])>_roiRadius){
			insideROI = false;
			break;
		}
	}

	if(insideROI == false)
        setROIpassive();
	else
        setROIactive();
		
}

void InteractionROI::setROIactive(){
    ptemplate.setSizeRange(osgParticle::rangef(0.0001f, 0.25f));
    ptemplate.setAlphaRange(osgParticle::rangef(0.1f, 0.0f));
    
    _ps->setDefaultAttributes("objects/roiParticleGreen-01.png", true, false);
}


void InteractionROI::setROIpassive(){
    ptemplate.setSizeRange(osgParticle::rangef(0.0001f, 0.25f));
    ptemplate.setAlphaRange(osgParticle::rangef(0.5f, 0.0f));
    _ps->setDefaultAttributes("objects/roiParticleRed-01.png", true, false);
}