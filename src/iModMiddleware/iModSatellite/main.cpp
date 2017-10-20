/**
* @file iModSatellite/main.cpp
*
* @brief Entry point for the Satellite
*
* This program is only used by Windows and is a headless client
*
*/

#include <QApplication>

#include "imodsatellite.h"

/**
* @brief Main function of the Daemon.
* @param argc Number of startparameters (unused)
* @param argv Startparameters (unused)
* @return Returns exitcode
*/
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    iModSatellite* satellite = new iModSatellite();
    satellite->connectDemon();

    return a.exec();
}

