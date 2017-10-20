/**
* @mainpage iModMiddleware
*
* <h3>Description:</h3>
* The project consists of multiple applications. The core of the project is the iModDaemon which contains the main logick for the project.<br>
* <ul>
* <li><b>iModDaemon</b> Central server of the project</li>
* <li><b>iModControl</b> Library containing the network communication protocoll between clients and the daemon. Additionally it has the implementation for the Kinect V2.</li>
* <li><b>iModView</b> Contains a GUI to control the daemon and change it's settings</li>
* <li><b>iModSatellite</b> A basic program which is used for Windows (8.1+) to grab trackingdata from a Kinect and pass it to the daemon</li>
* <li><b>iModNetwork</b> It is not a standallone Project. It only contains a central network class which is used by the daemon and the control library</li>
* </ul>
*
* <h3>Download</h3>
* It is possible to download precompiled versions of the Project for Windows and Linux. The installer makes use of the Qt-Installer-Framework. Along with the project there will be a maintenance tool to download updates from a remote repository. <br>
* Attention: The installers will only work, if you are in the 139.20.16.* or 139.20.18.* subnet. <br>
* <ul>
* <li><b>Windows 10 (x64 | msvc 12)</b> <a href="installer/windows_10_x64.zip">Download</a></li>
* <li><b>Arch Linux (x64 | gcc 5.3)</b> <a href="installer/linux_x64.tar.gz">Download</a></li>
* </ul>
*
* <h3>Compiling from source</h3>
* Compiling from source requires a Qt5.5 installation and the InteractionModels-Repository. Basicly there are two ways to compile from source. The first is to use the CMake-Script in ~/InteractionModels/CPP/. Alternatively you can build every single part of the project by using the qmake buildsystem. The required .pro files are located within every sourcefolder of the project parts.<br>
* Things you might need to compile the project:
* <ul>
* <li>Qt 5.5</li>
* <li>MySql client librarys</li>
* <li>Eigen 3</li>
* <li>Microsoft Kinect SDK 2.0 (Windows only)</li>
* </ul>
*
* <h3>Usage</h3>
* It is not possible to configure everything from within the iMod View. The biggest part of the configuration is locatet in the configuration.ini file. Most of the data shouldnt be changed, like the mapping of the Kinect targets or the tracking port, but you may want to change the ip address of the database server. To use the project, you need to start the iModDaemon (only one) in order to use the other programs. The clients will automaticaly find the daemon, if they are started in the same local network and UDP-Broadcast ist enabled. It is recommended to have an iMod View, to be able to configure the daemon. The View has a Tabview which groups the usecases of the daemon. Note, that only one client can edit the settings from the daemon at a time. To gain root privileges, the View can take it with the button "Forece Root". The satellites dont need any configuration by the user. After they are started (headless), they will connect to the daemon and start to push trackingdata from the Kinects. If you want to implement/use your own client software, please have a look into the documentaion of iModControll. It contains the interface when using the iMod Control library.<br>
* <img src="./images/iModMiddleware.png" alt="Overview"><br>
* The above image shows a common usecase. Green boxes show which components are covered within this project. The A.R.T.-Server and Kinects are able to track the user, but the kinects may be limited to a single user depending on the desired interaction. Interactions like dancing, where the users come close to each other may be problematic when the kinects are used. Furthermore trackingdata can be stored within the SQL-Server and be replayed later on.<br>
* Blender is used to reconstruct the human skeleton based on the joint movement tracked by the different trackingsystems. In addition Blender is able to compute an inverse kinematic for the UR5 robot. The data of the kinematics are transferred back to the daemon and is used to control the robot. This can be done by direclty connecting to the robot or by transfering the movementdata to ROS.
* <br><br><i>Description in main.cpp of Deamon</i>
*/

/**
* @file iModDaemon/main.cpp
* @brief Entry point for the Daemon
*/

#include <QtWidgets/QApplication>
#include <QtCore/QPointer>

//#include "imodcontroll.h"
#include "imoddaemon.h"
#include "imodutile.h"

//DEBUG
#include <QtNetwork/QTcpSocket>
#include "imodart.h"

/**
  * @brief The code that is emitted by the QApplication::quit(code) to show that the daemon has to restart instead of shutting down.
  */
#define RESTART_CODE 1000

/**
* @brief Main function of the Daemon.
* @param argc Number of startparameters (unused)
* @param argv Startparameters (unused)
* @return Returns exitcode
*
* It is able to restart the Daemon, if the right exitcode is set.
*/
int main(int argc, char *argv[])
{
  int returncode;
  QPointer<QApplication> app;
  QPointer<iModDaemon> mainwindow;
  QPointer<iModUtile> utile;
  do
  {
    if(app)
      delete app;
    if(mainwindow)
      delete mainwindow;
    if(utile)
      delete utile;

    app        = new QApplication(argc, argv);
    utile      = new iModUtile();
    mainwindow = new iModDaemon(utile, app);
    app->setQuitOnLastWindowClosed(false);
    mainwindow->startControll();

    returncode = app->exec();
  } while(returncode == RESTART_CODE);

  return returncode;
}
