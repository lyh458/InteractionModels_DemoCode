#ifndef HAPTICFEEDBACK_H_
#define HAPTICFEEDBACK_H_

#include <btBulletDynamicsCommon.h>
#include <iostream>


namespace iModHaptic {
	
	
	class HapticFeedback{
		
	public:
	HapticFeedback();
	virtual ~HapticFeedback();	
	
	/*! start and stop Feedback Wii */
	virtual void init()=0;
	virtual void startFeedbackLeft()=0;
	virtual void startFeedbackRight()=0;
	virtual void stoppFeedbackLeft()=0;
	virtual void stoppFeedbackRight()=0;
	
	/*! start and stop Feedback on every Finger of the data glove */
	virtual void startFeedbackLeftThumb()=0;
	virtual void startFeedbackRightThumb()=0;
	virtual void stoppFeedbackLeftThumb()=0;
	virtual void stoppFeedbackRightThumb()=0;

	virtual void startFeedbackLeftIndex()=0;
	virtual void startFeedbackRightIndex()=0;
	virtual void stoppFeedbackLeftIndex()=0;
	virtual void stoppFeedbackRightIndex()=0;
		
	virtual void startFeedbackLeftMiddle()=0;
	virtual void startFeedbackRightMiddle()=0;
	virtual void stoppFeedbackLeftMiddle()=0;
	virtual void stoppFeedbackRightMiddle()=0;
	
	/*! event Wii */
	virtual float is_pressed()=0;

	};
	
	
	
	
	
}
#endif /* HAPTICFEEDBACK_H_ */
