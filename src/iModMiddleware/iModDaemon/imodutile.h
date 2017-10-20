/**
* @file iModDaemon/imodutile.h
* @brief This file contains the utility class
*/

#ifndef UTILE_H
#define UTILE_H

#include <QObject>

// TODO: Update this thing to read new settings at runime -> sync?

class QFile;
class QSettings;
class QTimer;
/**
 * @class iModUtile
 * @brief iMod utility class providing functions to read from the config file
 */
class iModUtile : public QObject
{
    Q_OBJECT
  public:
    explicit iModUtile(QObject *parent = 0);
    ~iModUtile();

    void writeLog(QString message);
    /**
     * @brief Reads a setting from the logfile
     * @param path Key for desired value in logfile
     * @return Returns the value as QVariant. You need to convert it to the desired type on your own
     */
    QVariant readSetting(QString path);
    /**
     * @brief Gets all keys from a certain categors
     * @param category Categros
     * @return String list with all keys
     */
    QStringList getAllKeys(QString category);

  signals:

  private slots:
    /**
     * @brief This slot is used by the updatetimer to sync the daemon with the configfile. This writes changes made by the daemon and reads any changes made by externals in the configfile.
     */
    void slotUpdateSettingsFile();

  private:
    QSettings* settingsfile;
    QFile* logfile;
    QTimer* updatetimer;
};

#endif // UTILE_H
