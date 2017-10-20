#include "HapticFeedback.h"
#include "../interactionMesh/PoseContainer.h"
#include "../utils/Configuration.h"
#include "../utils/notify.h"

#include <boost/asio.hpp>
#include <boost/lexical_cast.hpp>
using boost::asio::ip::udp;



namespace iModHaptic {
class Udp : public HapticFeedback{
		
		
		
	public:
	udp::socket *socket;
	boost::asio::ip::udp::endpoint endpoint;
	boost::asio::io_service io_service;
	
	/*!
	 * Initialize the connection to the server
	 * @param port the Udp port in use
	 * @param ip the IP where ART-Server is running
	 * @param trial How often should the client try to establish a connection
	 */
	void UDP_socket(const int port, const std::string ip = "139.20.18.61", const int trials = 3);
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
