/*
 * SwitchNaoKeyboardEventHandler.h
 *
 *  Created on: Jan. 30, 2014
 *      Author: Mengzhi Zhang, Hongchang Hao
 */

#ifndef SWITCHNAOKEYBOARDEVENTHANDLER_H
#define SWITCHNAOKEYBOARDEVENTHANDLER_H
#include <osg/Switch>

// keyboard event handler to show each node with key 'n'
class SwitchNaoKeyboardEventHandler : public osgGA::GUIEventHandler
{
 public:

		SwitchNaoKeyboardEventHandler():
			_childNum(0) {}

		virtual bool handle(const osgGA::GUIEventAdapter& ea,osgGA::GUIActionAdapter& , osg::Object* object, osg::NodeVisitor* )
		{
			osg::Switch* sw = dynamic_cast<osg::Switch*>(object);
			if (!sw) return false;

			if (ea.getHandled()) return false;

			switch(ea.getEventType())
			{
				case(osgGA::GUIEventAdapter::KEYDOWN):
				{
					if (ea.getKey()=='n')
					{

						//switch  tube--->mesh--->none--->tube--->...
//						++_childNum;
//						if (_childNum > sw->getNumChildren()) _childNum = 0;
//						if (_childNum == sw->getNumChildren()) sw->setAllChildrenOff();
//						else sw->setSingleChildOn(_childNum);


						//switch  tube--->mesh--->tube--->...
						++_childNum;
						if (_childNum >= sw->getNumChildren()) _childNum = 0;

						sw->setSingleChildOn(_childNum);

					return true;
					}
					break;
				}
				default:
					break;
			}
			return false;
		}

	private:

		virtual ~SwitchNaoKeyboardEventHandler() {}
		unsigned int    _childNum;
};


#endif // SWITCHNAOKEYBOARDEVENTHANDLER_H
