#include "HapticFeedback.h"
 #include "bluetooth.h"
#include "../../external/wiiuse-master/src/wiiuse.h"
 



namespace iModHaptic {
class Wii : public HapticFeedback{
		
	public:
		
		void init();
		virtual void startFeedbackLeft();
		virtual void startFeedbackRight();
		virtual void stoppFeedbackLeft();
		virtual void stoppFeedbackRight();

		virtual void startFeedbackLeftThumb();
		virtual void startFeedbackRightThumb();
		virtual void stoppFeedbackLeftThumb();
		virtual void stoppFeedbackRightThumb();
		
		virtual void startFeedbackLeftIndex();
		virtual void startFeedbackRightIndex();
		virtual void stoppFeedbackLeftIndex();
		virtual void stoppFeedbackRightIndex();
		
		virtual void startFeedbackLeftMiddle();
		virtual void startFeedbackRightMiddle();
		virtual void stoppFeedbackLeftMiddle();
		virtual void stoppFeedbackRightMiddle();
		
			virtual float is_pressed();
	
};


}
