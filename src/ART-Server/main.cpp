#include "art.h"
#include <QApplication>
#include <QtCore/QCommandLineParser>

int main(int argc, char *argv[])
{
#ifdef CONSOLE_MODE
  QCoreApplication a(argc, argv);
  QCoreApplication::setApplicationName("ART-Server");
  QCoreApplication::setApplicationVersion("2.0");
  QCommandLineParser parser;
  parser.setApplicationDescription("This program is able to simulate the A.R.T tracking server. It cant use a database or the Kinect for Windows to get input for simulation");
  parser.addHelpOption();
  parser.addVersionOption();

  QCommandLineOption tableOption(QStringList() << "t" << "tables",
                                 QCoreApplication::translate("main", "Loads all mentioned tables, seperatet by '#'."),
                                 QCoreApplication::translate("main", "names"));
  parser.addOption(tableOption);
  parser.process(a);
  QString tables = parser.value(tableOption);

  ART w(tables);

#else
  QApplication a(argc, argv);
  QApplication::setApplicationName("ART-Server");
  QApplication::setApplicationVersion("2.0");
  ART w;
  w.show();
#endif

  return a.exec();
}
