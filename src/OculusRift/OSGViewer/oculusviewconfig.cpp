/*
 * oculusviewconfig.cpp
 *
 *  Created on: Sept 26, 2013
 *      Author: Bjorn Blissing & Jan Ciger
 */
#include "oculusviewconfig.h"
#include <QtCore/QDebug>
#include "oculuseventhandler.h" 
#include <iostream>

void OculusViewConfig::configure(osgViewer::View& view) const
{
	// Create a graphic context based on our desired traits
	osg::ref_ptr<osg::GraphicsContext::Traits> traits = m_device->graphicsContextTraits();
	osg::ref_ptr<osg::GraphicsContext> gc = osg::GraphicsContext::createGraphicsContext(traits);
	if (!gc) {
		osg::notify(osg::NOTICE) << "Error, GraphicsWindow has not been created successfully" << std::endl;
		return;
	}
	
	// Attach a callback to detect swap
	osg::ref_ptr<OculusSwapCallback> swapCallback = new OculusSwapCallback(m_device);
	gc->setSwapCallback(swapCallback);

	osg::ref_ptr<osg::Camera> camera = view.getCamera();
	camera->setName("Main");
	// Disable scene rendering for main camera
	camera->setCullMask(~m_sceneNodeMask);
	camera->setGraphicsContext(gc);
	// Use full view port
	camera->setViewport(new osg::Viewport(0, 0, traits->width, traits->height));
	// Disable automatic computation of near and far plane on main camera, will propagate to slave cameras
	camera->setComputeNearFarMode( osg::CullSettings::DO_NOT_COMPUTE_NEAR_FAR );
	const int textureWidth  = m_device->renderTargetWidth()/2;
	const int textureHeight = m_device->renderTargetHeight();
	// master projection matrix
	camera->setProjectionMatrix(m_device->projectionMatrixCenter());
	// Create textures for RTT cameras
	osg::ref_ptr<osg::Texture2D> textureLeft = new osg::Texture2D;
	textureLeft->setTextureSize( textureWidth, textureHeight );
	textureLeft->setInternalFormat( GL_RGBA );
	osg::ref_ptr<osg::Texture2D> textureRight = new osg::Texture2D;
	textureRight->setTextureSize( textureWidth, textureHeight );
	textureRight->setInternalFormat( GL_RGBA );
	// Create RTT cameras and attach textures
	osg::ref_ptr<osg::Camera> cameraRTTLeft = m_device->createRTTCamera(textureLeft, OculusDevice::LEFT, osg::Camera::RELATIVE_RF, gc);
	osg::ref_ptr<osg::Camera> cameraRTTRight = m_device->createRTTCamera(textureRight, OculusDevice::RIGHT, osg::Camera::RELATIVE_RF, gc);
	cameraRTTLeft->setName("LeftRTT");
	cameraRTTRight->setName("RightRTT");
	cameraRTTLeft->setCullMask(m_sceneNodeMask);
	cameraRTTRight->setCullMask(m_sceneNodeMask);
	
	// Create warp ortho camera
	osg::ref_ptr<osg::Camera> cameraWarp = m_device->createWarpOrthoCamera(0.0, 1.0, 0.0, 1.0, gc);
	cameraWarp->setName("WarpOrtho");
	cameraWarp->setViewport(new osg::Viewport(0, 0, m_device->screenResolutionWidth(), m_device->screenResolutionHeight()));

	// Create shader program
	osg::ref_ptr<osg::Program> program = m_device->createShaderProgram();

	// Create distortionMesh for each camera
	osg::ref_ptr<osg::Geode> leftDistortionMesh = m_device->distortionMesh(OculusDevice::LEFT, program, 0, 0, textureWidth, textureHeight);
	cameraWarp->addChild(leftDistortionMesh);

	osg::ref_ptr<osg::Geode> rightDistortionMesh = m_device->distortionMesh(OculusDevice::RIGHT, program, 0, 0, textureWidth, textureHeight);
	cameraWarp->addChild(rightDistortionMesh);

	// Add pre draw camera to handle time warp
	cameraWarp->setPreDrawCallback(new WarpCameraPreDrawCallback(m_device));

	// Attach shaders to each distortion mesh
	osg::ref_ptr<osg::StateSet> leftEyeStateSet = leftDistortionMesh->getOrCreateStateSet();
	osg::ref_ptr<osg::StateSet> rightEyeStateSet = rightDistortionMesh->getOrCreateStateSet();

	m_device->applyShaderParameters(leftEyeStateSet, program.get(), textureLeft.get(), OculusDevice::LEFT);
	m_device->applyShaderParameters(rightEyeStateSet, program.get(), textureRight.get(), OculusDevice::RIGHT);

	// Add RTT cameras as slaves, specifying offsets for the projection
	view.addSlave(cameraRTTLeft, 
		m_device->projectionOffsetMatrixLeft(),
		m_device->viewMatrixLeft(), 
		true);
	view.addSlave(cameraRTTRight, 
		m_device->projectionOffsetMatrixRight(),
		m_device->viewMatrixRight(),
		true);

	// Add warp camera as slave
	view.addSlave(cameraWarp, false);
	view.setName("Oculus");

	// Connect main camera to node callback that get HMD orientation
	camera->setDataVariance(osg::Object::DYNAMIC);
	camera->setUpdateCallback(new OculusViewConfigOrientationCallback(cameraRTTLeft, cameraRTTRight, m_device, swapCallback));

	// Add Oculus keyboard handler
	view.addEventHandler(new OculusEventHandler(m_device));
}

void OculusViewConfigOrientationCallback::operator() (osg::Node* node, osg::NodeVisitor* nv)
{
	osg::Camera* mainCamera = static_cast<osg::Camera*>(node);
	osg::View* view = mainCamera->getView();

	if (view) {
		m_device.get()->updatePose(m_swapCallback->frameIndex());
		osg::Vec3 position = m_device.get()->position();
		osg::Quat orientation = m_device.get()->orientation();
		osg::Matrix viewOffsetLeft = m_device.get()->viewMatrixLeft();
		osg::Matrix viewOffsetRight = m_device.get()->viewMatrixRight();
		viewOffsetLeft.preMultRotate(orientation);
		viewOffsetRight.preMultRotate(orientation);
		viewOffsetLeft.preMultTranslate(position);
		viewOffsetRight.preMultTranslate(position);
		// Nasty hack to update the view offset for each of the slave cameras
		// There doesn't seem to be an accessor for this, fortunately the offsets are public
		view->findSlaveForCamera(m_cameraRTTLeft.get())->_viewOffset = viewOffsetLeft;
		view->findSlaveForCamera(m_cameraRTTRight.get())->_viewOffset = viewOffsetRight;
	}

	traverse(node, nv);
}

