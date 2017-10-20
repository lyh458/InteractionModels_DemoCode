/*
 * VirtualHuman.cpp
 *
 *  Created on: Oct 30, 2013
 *     Authors: ddvogt, bjung
 */

#include "VirtualHuman.h"
#include "VirtualHumanRigTransformHardware.h"
#include "NodeVisitorCallbackRunner.h"

#include <typeinfo>       // std::bad_cast
#include <osg/PositionAttitudeTransform>
#include <osg/BlendFunc> 

VirtualHuman::VirtualHuman() {
    _rootMatrix = osg::Matrix::identity();
}

VirtualHuman::~VirtualHuman() {
    // TODO Auto-generated destructor stub
}

void VirtualHuman::removeLightSource(){

	// remove any light sources from the loaded model;
	// local light sources might interfere global lighting of the scene, e.g. overwrite the viewer's HEADLIGHT
	// unfortunately, we can't delete nodes while a node visitor is running (leads to seg faults)
	std::vector<osg::LightSource*> lightsources; // list all light sources here
	std::function<bool (osg::Node& node)> nodecallback = [&lightsources] (osg::Node& node) // capture this for _currentRotations
	{
		if (!strcmp(node.className(), "LightSource"))
		{
			osg::LightSource* ls = (osg::LightSource*) &node;
			lightsources.push_back(ls);
			return false; // we don't need to traverse the lightsource's children.
		}
		return true;
	};

	// VH's only cast shadow, disable culling
	std::function<bool (osg::Geode& geode)> geodecallback = [] (osg::Geode& geode) // capture the hardware variable by reference
	{
		// culling off
		geode.setCullingActive(false);
		// castShadowMask 0x41
		geode.setNodeMask(0x0101);
		return true;
	};

	// Off you go, visitor! Pls call back.
	NodeVisitorCallbackRunner cbRunner(nodecallback, geodecallback);
	_model.get()->accept(cbRunner);

	// now, remove all of the lightsources.
	for (int i = 0; i < lightsources.size(); i++) {
		osg::LightSource* lightsource = lightsources[i];
		osg::Node::ParentList lightsourceparents = lightsource->getParents();
		for (int k = 0; k < lightsourceparents.size(); k++) {
			lightsourceparents[k]->removeChild(lightsource);
		}
	}
}

void VirtualHuman::load(std::string fileNameAndPath){

	_model = osgDB::readNodeFile(fileNameAndPath);
	if (!_model) {
		osg::notify(osg::FATAL)<< "Could not load the virtual human model: " << fileNameAndPath << std::endl;
		return ;
	}
	removeLightSource();

    // osg::StateSet* state2 = _model->getOrCreateStateSet(); 
    // osg::ref_ptr<osg::Material> mat2 = new osg::Material;
    // mat2->setAlpha(osg::Material::FRONT_AND_BACK, 0.1); 
    // state2->setAttributeAndModes( mat2.get() , osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
    // osg::BlendFunc* bf = new osg::BlendFunc(osg::BlendFunc::SRC_ALPHA, 
    // osg::BlendFunc::ONE_MINUS_SRC_ALPHA ); 
    // state2->setAttributeAndModes(bf);
}

void VirtualHuman::setup(osg::ref_ptr<osg::Group> root, osg::Vec3 defaultPos, osg::Quat defaultRot) {
    bool hardware = true; // set to false for sw skinning, true for hw skinning

    //Add translation animation callback
    //Tranformation to be manipulated by the animation
    osg::ref_ptr<osg::MatrixTransform> trans = new osg::MatrixTransform();
    trans->setName("TranslationAnimationNode");
    
    //Dynamic object, has to be updated during update traversal
    trans->setDataVariance(osg::Object::DYNAMIC);
    
    //Animation callback for Matrix transforms, name is targetName for Channels
    osgAnimation::UpdateMatrixTransform* updatecb = new osgAnimation::UpdateMatrixTransform("TranslationAnimationCallback");

    //add manipulator Stack, names must match with channel names
    //elements are applied in LIFO order
    //The first element modifies the position component of the matrix
    updatecb->getStackedTransforms().push_back(new osgAnimation::StackedMatrixElement("rootTransformation",osg::Matrixd::identity()));

    //connect the UpdateMatrixTransform callback to the MatrixTRanform
    trans->setUpdateCallback(updatecb);
    
    //initialize MatrixTranform
    trans->setMatrix(osg::Matrix::identity());
    
    //set up location and rotation offsets
    osg::ref_ptr<osg::PositionAttitudeTransform> t = new osg::PositionAttitudeTransform();
    t->setAttitude(defaultRot);
    t->setPosition(defaultPos);
    t->addChild(_model.get());

    //append character node
    trans->addChild(t);
    root->addChild (trans.get());
    
    // prepare a function for setting the new bone update callbacks.
    std::function<bool (osg::Node& node)> nodecallback = [this] (osg::Node& node) // capture this for _currentRotations
    {
        osgAnimation::Bone* bone = dynamic_cast<osgAnimation::Bone*>(&node);
        if (bone)
        {
            // Initiale Translation und Rotation (Angle, Axis) des Knochen auslesen (aus Original-Callback in osgt-Datei)
            osgAnimation::UpdateBone* loadedUpdateBone = (osgAnimation::UpdateBone*) bone->getUpdateCallback();
            loadedUpdateBone->getStackedTransforms().update();
            osg::Vec3 loadedBoneTrans = loadedUpdateBone->getStackedTransforms().getMatrix().getTrans();
            osg::Quat loadedBoneRot = loadedUpdateBone->getStackedTransforms().getMatrix().getRotate();

            osg::Matrixf loadedMatrix=loadedUpdateBone->getStackedTransforms().getMatrix()
                    *loadedUpdateBone->getStackedTransforms().getMatrix();

            // neuen Callback erzeugen, mit gleicher Rotation und Translation, aber eigenen Benennungen
            osgAnimation::UpdateBone* boneUpdate = new osgAnimation::UpdateBone(bone->getName());

            boneUpdate->getStackedTransforms().push_back(new osgAnimation::StackedTranslateElement("translateDefault",loadedBoneTrans));
            boneUpdate->getStackedTransforms().push_back(new osgAnimation::StackedQuaternionElement("rotateDefault", loadedBoneRot));  // Default-Rotation des Knochen
            //boneUpdate->getStackedTransforms().push_back(new osgAnimation::StackedMatrixElement("matrixDefault", loadedMatrix));

            boneUpdate->getStackedTransforms().push_back(new osgAnimation::StackedQuaternionElement("rotate", osg::Quat()));    // durch Nutzer steuerbar
            boneUpdate->getStackedTransforms().push_back(new osgAnimation::StackedTranslateElement("translate", osg::Vec3()));  // durch Nutzer steuerbar
            boneUpdate->getStackedTransforms().push_back(new osgAnimation::StackedMatrixElement("matrix",osg::Matrixf()));

            bone->setUpdateCallback(boneUpdate);

            // add the new bone to map
            _currentRotations.insert( std::pair<std::string,osg::Quat>( bone->getName(), osg::Quat( 0.0, osg::X_AXIS) ) );
        }
        return true;
    };

    // now, set hardware rendering. See NodeVisitorCallbackRunner.h for example.
    std::function<bool (osg::Geode& geode)> geodecallback = [&hardware] (osg::Geode& geode) // capture the hardware variable by reference
    {
        // to hardwarerender or not to hardwarerender this drawable, that is the question
        for (auto i = 0; i < geode.getNumDrawables(); i++)
        {
            osgAnimation::RigGeometry* rig = dynamic_cast<osgAnimation::RigGeometry*>(geode.getDrawable(i));
            if (!rig)
            {
                continue;
            }

            hardware ? rig->setRigTransformImplementation(new VirtualHumanRigTransformHardware)
             : rig->setRigTransformImplementation(new osgAnimation::RigTransformSoftware);
        }
        return true;
    };

    // start the visitor. it will run both callbacks for each node/geode.
    NodeVisitorCallbackRunner cbRunner(nodecallback, geodecallback);
    root->accept(cbRunner);

    _manager = new osgAnimation::BasicAnimationManager;
    root->setUpdateCallback(_manager.get());

    _currentRotationAnimation = new osgAnimation::Animation; // pro forma
}

void VirtualHuman::setPosture(const std::vector<std::string> &boneNames, const std::vector<osg::Quat> &rotations, std::vector<double> &times){

    //osg::notify(osg::NOTICE)<<"Got new rotations. Creating animation."<<std::endl;

    osgAnimation::Animation* rotAnim = new osgAnimation::Animation;

    for (int i = 0; i < boneNames.size(); i++) {
        osg::Quat oldposture;

        //Consider root rotation
        //This seems to be a hack. How can one integrate the initial PAT rotation for the virtual human?
        //f(boneNames[i] == "HumanoidRoot"){
        //    oldposture = _currentRotations[ boneNames[i] ] * _defaultRotat;
        //}
        //else{
            oldposture = _currentRotations[ boneNames[i] ];
        //}
        osg::Quat newposture = rotations[i];
        _currentRotations[ boneNames[i] ] = newposture;

        osgAnimation::QuatKeyframeContainer* keys0 = new osgAnimation::QuatKeyframeContainer;
        keys0->push_back(osgAnimation::QuatKeyframe(0.0f, oldposture));
        keys0->push_back(osgAnimation::QuatKeyframe(times[i], newposture));

        osgAnimation::QuatSphericalLinearSampler* sampler = new osgAnimation:: QuatSphericalLinearSampler;
        sampler->setKeyframeContainer(keys0);
        osgAnimation::QuatSphericalLinearChannel* channel = new osgAnimation::QuatSphericalLinearChannel(sampler);

        channel->setName("rotate");

        channel->setTargetName(boneNames[i]);
        rotAnim->addChannel(channel);
    }

    rotAnim->setPlayMode(osgAnimation::Animation::ONCE);

    updateAnimation(rotAnim,"rotate");

}

void VirtualHuman::setRootTransformation(osg::Quat &rot, osg::Vec3 &position, double &duration){
    osgAnimation::Animation* anim = new osgAnimation::Animation;

    osg::Matrixf oldMat = _rootMatrix;

    _rootMatrix.makeIdentity();
    _rootMatrix.setTrans(position);
    _rootMatrix.setRotate(rot);

    //! Create a new matrix animation
    osgAnimation::MatrixKeyframeContainer *container = new osgAnimation::MatrixKeyframeContainer;
    container->push_back(osgAnimation::MatrixKeyframe(0.0,oldMat));
    container->push_back(osgAnimation::MatrixKeyframe(duration, _rootMatrix));

    osgAnimation::MatrixLinearSampler* sampler = new osgAnimation::MatrixLinearSampler;
    sampler->setKeyframeContainer(container);
    osgAnimation::MatrixLinearChannel* channel = new osgAnimation::MatrixLinearChannel(sampler);

    channel->setName("rootTransformation");
    channel->setTargetName("TranslationAnimationCallback");
    anim->addChannel(channel);

    anim->setPlayMode(osgAnimation::Animation::ONCE);
    updateAnimation(anim,channel->getName());

    /* Rotate around a point:
    osg::Matrixf oldMat = _rootMatrix;
    rotation.get(_rootMatrix);

    osg::Matrixf t,tInvert;
    t.makeIdentity();
    t.translate(pivot);
    t.invert(tInvert); //We assume that an inverse matrix exists

    _rootMatrix =  tInvert * _rootMatrix * t;
    */
}

void VirtualHuman::updateAnimation(osgAnimation::Animation* anim, std::string name){

    osgAnimation::Animation* cA = _currentAnimation[name];

    _manager->unregisterAnimation(cA);
    _currentAnimation[name] = anim;
    _manager->registerAnimation(anim);
    _manager->playAnimation(anim);
}
