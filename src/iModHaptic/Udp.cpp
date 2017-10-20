#include "Udp.h"
#include <boost/cerrno.hpp>

using namespace iModHaptic;





 



 void Udp::UDP_socket(int port, std::string ip, int trials){


    

 
	try{
   
		
		udp::resolver resolver(io_service);
		udp::resolver::query query(ip, boost::lexical_cast<std::string>(port) );
		udp::resolver::iterator endpoint_iterator = resolver.resolve(query);
    
    
    
		//receiver_endpoint = *resolver.resolve(query);

		socket=new udp::socket(io_service);
		boost::system::error_code error = boost::asio::error::host_not_found;
		socket->close(error);
		socket->open(udp::v4());
	
	
         
	while (endpoint_iterator != udp::resolver::iterator()){

			//boost::asio::ip::udp::endpoint endpoint;
			endpoint = *endpoint_iterator++;
			UT_NOTIFY(LV_INFO,"Blender Controller: Endpoint "<<endpoint);
			socket->connect(endpoint, error);
			
		   
	}
		
		//Do some basic connection error handling
		if (error != 0){
			UT_NOTIFY(LV_INFO,"Blender Controller connection error. Retrying. ");//If the connection has been refused then wait a bit and try again
			if(error == boost::asio::error::connection_refused && trials > 0){
				boost::this_thread::sleep(boost::posix_time::seconds(1));
				UDP_socket(port,ip,--trials);
			}
			else throw boost::system::system_error(error);
		}
		UT_NOTIFY(LV_INFO,"Connection to "<<ip<<" created successfully.");
	
	}
	 	
	catch (boost::system::system_error& e) {
		UT_NOTIFY(LV_ERROR,"Initialize Blender error: "<<e.what());
	}catch (std::exception& e) {
		UT_NOTIFY(LV_ERROR,"Initialize Blender error: "<<e.what());
	}


}
	
void Udp::init(){
	std::string ARTHost =iMod::Configuration::getInstance().getParameter<std::string>("ART","ServerAdress");
		Udp::UDP_socket(50110, "139.20.18.61",1); /*! Port 50110 is needed*/
}	
	
	
float Udp::is_pressed(){}	
	
	
	
	
	/*!
	 * send Sting "tfb 1 [bodyid fingerid penetrationDepth strength]" to ART-Server
	 * bodyid: select data glove
	 * fingerid: select finger
	 * penetrationdepth: start, stopp Feedback
	 * strength: select Feedback strength
	 */
void Udp::startFeedbackLeft(){

		std::string message = "tfb 1 [1 0 1 1]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);
}
void Udp::startFeedbackRight(){

      	std::string message = "tfb 1 [0 0 1 1]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);
    

	
}
void Udp::stoppFeedbackLeft(){

          	std::string message = "tfb 1 [1 0 0 0.]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);
          

}

void Udp::stoppFeedbackRight(){
		std::string message = "tfb 1 [0 0 0 0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);
}
void Udp::startFeedbackLeftThumb(){
	
			std::string message = "tfb 1 [1 0 1.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);

	//fingerID: 0=thumb, 1=index-finger, 2=middle-finger,

}
void Udp::startFeedbackRightThumb(){
		std::string message = "tfb 1 [0 0 1.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);
      
    

	
}
void Udp::stoppFeedbackLeftThumb(){
	std::string message = "tfb 1 [1 0 0.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);
          

}

void Udp::stoppFeedbackRightThumb(){
		std::string message = "tfb 1 [0 0 0.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);

}

void Udp::startFeedbackLeftIndex(){
	
	 
			std::string message = "tfb 1 [1 1 1.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);

	//fingerID: 0=thumb, 1=index-finger, 2=middle-finger,

}
void Udp::startFeedbackRightIndex(){
		std::string message = "tfb 1 [0 1 1.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);
      
    

	
}
void Udp::stoppFeedbackLeftIndex(){
	std::string message = "tfb 1 [1 1 0.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);
          

}

void Udp::stoppFeedbackRightIndex(){
		std::string message = "tfb 1 [0 1 0.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);

}

void Udp::startFeedbackLeftMiddle(){
	
	 
			std::string message = "tfb 1 [1 2 1.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);

	//fingerID: 0=thumb, 1=index-finger, 2=middle-finger,

}
void Udp::startFeedbackRightMiddle(){
		std::string message = "tfb 1 [0 2 1.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);
      
    

	
}
void Udp::stoppFeedbackLeftMiddle(){
	std::string message = "tfb 1 [1 2 0.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
          endpoint, 0, ignored_error);
          

}

void Udp::stoppFeedbackRightMiddle(){
		std::string message = "tfb 1 [0 2 0.0 1.0]";//middle-finger
      boost::system::error_code ignored_error;
      socket->send_to(boost::asio::buffer(message),
      endpoint, 0, ignored_error);

}
