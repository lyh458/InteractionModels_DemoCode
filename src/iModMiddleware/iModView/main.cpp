/**
* @file View/main.cpp
*
* @brief Entry point for the View
*
* This program provides the main GUI to controll various functions of the Daemon
*
*/

#include "imodview.h"
#include <QApplication>

/**
* @brief Main function of the View.
* @param argc Number of startparameters (unused)
* @param argv Startparameters (unused)
* @return Returns exitcode
*/
int main(int argc, char *argv[])
{
  QApplication a(argc, argv);
  iModView w;
  w.show();

  return a.exec();
}
