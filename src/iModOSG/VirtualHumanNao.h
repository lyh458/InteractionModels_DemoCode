/*
 * VirtualHumanNao.h
 *
 *  Created on: Feb. 06 2013
 *      Author: Mengzhi Zhang, Hongchang Hao
 */

#ifndef VIRTUALHUMANNAO_H_
#define VIRTUALHUMANNAO_H_

#include "VirtualHuman.h"
#include "BoneFinder.h"
#include "BoneGen.h"
#include "CylinderCreator.h"
#include "SwitchNaoKeyboardEventHandler.h"
#include "VirtualHumanNao.h"

class VirtualHumanNao: public VirtualHuman::VirtualHuman {

	int _currentVisibleChild;
	static int _childNum;

public:
	VirtualHumanNao();
	virtual ~VirtualHumanNao();

	/*!
	 * Load a Nao.osgt model.
	 * */
	void loadNaoTube();

	/*!
	 * Load a NaoRigShape.osgt model.
	 * */
	void loadNaoMesh();

};


#endif /* VIRTUALHUMANNAO_H_ */
