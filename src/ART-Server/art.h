#ifndef ART_H
#define ART_H

#ifndef CONSOLE_MODE
#include <QMainWindow>
#else
#include <QtCore/QObject>
#include <QtCore/QStringList>
#endif

#include <QtCore/QMutex>

//#include "artkinectv1.h"

namespace Ui {
class ART;
}

class QLabel;
class ARTKinectV1;
class ARTKinectV2;
class QThread;
class ARTKinectWidget;
class ARTNetwork;
class ARTDatabase;
class ARTConfiguration;
class QTimer;

#ifdef CONSOLE_MODE
class ART : public QObject
#else
class ART : public QMainWindow
#endif
{
    Q_OBJECT

public:
#ifdef CONSOLE_MODE
    explicit ART(QString tables);
#else
    explicit ART(QWidget *parent = 0);
#endif

    ~ART();

  private:
    QString readSetting(QString value);
    QMap<int, QString> loadBoneMapping(int version);

  private slots:
    void slotKinectStarted();
    void slotKinectError(QString s);
    void slotKinectStopped();
    void slotStartKinect();
    void slotDisplayLogMessage(QString s);
    void slotFilledBuffer(bool success);
    void slotSetTableList(QStringList list);
    //void slotTest(double d) { qDebug() << d; }
    void slotSetClientNumber(int number);
    void slotArtFpsUpdate(int fps);

    void on_tabWidget_currentChanged(int index);
    void on_startButton_clicked();
    void on_loadTabelsButton_clicked();
    void on_AddTableButton_clicked();
    void on_RemoveTableButton_clicked();
    void on_TableUpButton_clicked();
    void on_TableDownButton_clicked();
    void on_acceptTableButton_clicked();
    void on_resetBuffer_clicked();
    void on_framerateSpinBox_valueChanged(int arg1);
    void on_kinectNearSlider_valueChanged(int value);
    void on_kinectFarSlider_valueChanged(int value);
    void on_checkBox_stateChanged(int arg1);
    void on_oculusCalibrationButton_clicked();
    void on_filterRiftCheckBox_stateChanged(int arg1);
    void on_oculusResetButton_clicked();
    void on_connect_art_button_clicked();
    void on_kinect_merge_data_clicked(bool checked);

  signals:
    void signalFillBuffer(QStringList);
    void signalClearBuffer();
    void signalResetBuffer();
    void signalAddClient(int, QString);
    void signalRemoveClient(int);
    void signalConnectDatabase(QString, QString, QString, QString);
    void signalGetAllTables();
    void signalStartKinect();
    void signalSetMaxDistance(double);
    void signalSetMinDistance(double);
    void signalUpdateOculusFilter(QString);
    void signalUpdateOculusCalibration();
    void signalResetOculusCalibration();
    void signalConnectARTServer(int, int, QString);
    void signalReadOculusTracking(int, bool);

  private:
    Ui::ART *ui;
    QLabel* kinectstate;
    QLabel* sqlstate;
    ARTKinectV1* kinectv1;
    ARTKinectV2* kinectv2;
    QThread* kinectthread;
    ARTKinectWidget* kinectview;
    ARTNetwork* artnetwork;
    QThread* networkthread;
    ARTDatabase* database;
    QThread* databasethread;
    QTimer* frametimer;
    QString cmtables;
    ARTConfiguration* configuration;
};

#endif // ART_H
