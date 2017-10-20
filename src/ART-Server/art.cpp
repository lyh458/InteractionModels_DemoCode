#include "art.h"
#ifndef CONSOLE_MODE
#include "ui_art.h"
#endif

#include <QtWidgets/QLabel>
#include <QtCore/QThread>
#include <QtCore/QDebug>
#include <QtCore/QDir>
#include <QtCore/QTimer>
#include <QtWidgets/QComboBox>
#include <QtCore/QTimer>
#include <QtCore/QSettings>
#include <QtWidgets/QInputDialog>
#include <QtCore/QCoreApplication>

#include "artkinectwidget.h"
#include "artnetwork.h"
#include "artdatabase.h"
#include "artkinectv1.h"
#include "artkinectv2.h"
#include "artconfiguration.h"

#ifdef CONSOLE_MODE
#include <QtCore/QDebug>

ART::ART(QString tables) :
  QObject(),
  cmtables(tables),
#else
ART::ART(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::ART),
#endif
  kinectv1(NULL),
  kinectv2(NULL),
  artnetwork(NULL),
  databasethread(NULL)
{

#ifndef CONSOLE_MODE
  ui->setupUi(this);
  kinectstate = new QLabel(ui->statusBar);
  sqlstate    = new QLabel(ui->statusBar);
  ui->statusBar->addWidget(sqlstate);
  ui->statusBar->addWidget(kinectstate);

  ui->oculusCalibrationButton->setEnabled(false);
  ui->checkBox->setEnabled(false);
  ui->filterRiftCheckBox->setEnabled(false);
  ui->oculusResetButton->setEnabled(false);
  ui->kinect_merge_data->setEnabled(false);

  kinectstate->setText("Kinect: not connected");
  sqlstate->setText("Database: not connected");

  configuration = new ARTConfiguration(ui->configurationTab);
  ui->scrollArea->resize(QSize(625,365));
  ui->scrollArea->move(QPoint(-1, -1));
  connect(configuration, &ARTConfiguration::signalDisplayLogMessage,
          this,          &ART::slotDisplayLogMessage);
  ui->scrollArea->setWidget(configuration);
#endif
#ifdef WIN32
  kinectview = new ARTKinectWidget(ui->kinectTab, configuration->readSetting("iModMiddleware/FullBodyWithKinect").toBool());
  kinectview->setDisplayText("Klick to start");

  connect(kinectview, &ARTKinectWidget::signalStartKinect,
          this,       &ART::slotStartKinect);
#endif
  database = new ARTDatabase();
  connect(database, &ARTDatabase::signalDisplayLogMessage,
          this,     &ART::slotDisplayLogMessage);
  connect(this,     &ART::signalFillBuffer,
          database, &ARTDatabase::slotFillBuffer);
  connect(database, &ARTDatabase::signalBufferFinished,
          this,     &ART::slotFilledBuffer);
  connect(this,     &ART::signalClearBuffer,
          database, &ARTDatabase::slotClearBuffer);
  connect(this,     &ART::signalResetBuffer,
          database, &ARTDatabase::slotResetBuffer);
  connect(this,     &ART::signalConnectDatabase,
          database, &ARTDatabase::slotConnectDatabase);
  connect(database, &ARTDatabase::signalSetTableList,
          this,     &ART::slotSetTableList);
  connect(this,     &ART::signalGetAllTables,
          database, &ARTDatabase::slotGetAllTables);

  databasethread = new QThread();
  database->moveToThread(databasethread);
  connect(databasethread, &QThread::finished,
          database,       &QObject::deleteLater);
  databasethread->start();

  frametimer = new QTimer(this);
  frametimer->setInterval(1000/readSetting("ART/refreshRate").toInt());
  connect(frametimer, &QTimer::timeout,
          database,   &ARTDatabase::slotSendFrame);

#ifdef CONSOLE_MODE
  on_loadTabelsButton_clicked();
  on_startButton_clicked();
#else
  ui->kinectNearLabel->setText("0.50 m");
  ui->kinectFarLabel->setText("8.00 m");
  ui->kinectComboBox->addItem("Kinect v1");
  ui->kinectComboBox->addItem("Kinect v2");
  ui->kinectComboBox->setCurrentIndex(0);
  QRegExp  rex("([0-9]{1,2};)*"); // IP: ^[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}:[0-9]{1,5}$
  rex.setPattern("([0-9]{1,2};)*");
  ui->oculus_filter_lineEdit->setValidator(new QRegExpValidator(rex, this));
  ui->progressBar->setRange(0,0);
  ui->progressBar->setVisible(false);
#endif
}

ART::~ART()
{
#ifndef CONSOLE_MODE
  delete ui;
#endif
}

void ART::slotKinectStarted()
{
#ifndef CONSOLE_MODE
  this->kinectstate->setText("Kinect: ready");
  slotDisplayLogMessage("Kinect ready");
  if(ui->tabWidget->currentIndex() == 1)
    ui->startButton->setEnabled(true);
  ui->progressBar->setVisible(false);
#endif
}

void ART::slotKinectStopped()
{
  this->kinectstate->setText("Kinect: not connected");
}

void ART::slotKinectError(QString s)
{
#ifdef WIN32
  this->kinectstate->setText("Kinect: " + s);
  kinectview->setDisplayText("Error: " + s + "\nKlick to retry");
  kinectview->update();
  ui->progressBar->setVisible(false);
#endif
}

void ART::slotStartKinect()
{
#ifdef WIN32
  kinectview->setDisplayText("");
  kinectview->update();
  ui->kinectComboBox->setEnabled(false);
  ui->progressBar->setVisible(true);

  if(ui->kinectComboBox->currentText() == "Kinect v1")
  {
    kinectview->setKinectVersion(1);
    if(kinectv1 != NULL)
    {
      slotDisplayLogMessage("Kinect already running!");
      ui->progressBar->setVisible(false);
      return;
    }
    slotDisplayLogMessage("Starting Kinect v1 ...");

    QMap<int, QString> mapping = loadBoneMapping(1);
    if(mapping.empty())
      return;
    kinectv1 = new ARTKinectV1(mapping, configuration->readSetting("iModMiddleware/FullBodyWithKinect").toBool());
    connect(frametimer, &QTimer::timeout,
            kinectv1,   &ARTKinectV1::slotSendFrame);

    kinectthread = new QThread();
    kinectv1->moveToThread(kinectthread);

    connect(kinectthread, &QThread::finished,
            kinectv1,     &QObject::deleteLater);
    connect(kinectv1,   &ARTKinectV1::signalDrawJoint,
            kinectview, &ARTKinectWidget::slotDrawJointV1);
    connect(kinectv1, &ARTKinectV1::signalKinectRunning ,
            this,     &ART::slotKinectStarted);
    connect(kinectv1, &ARTKinectV1::signalKinectStopped ,
            this,     &ART::slotKinectStopped);
    connect(kinectv1, &ARTKinectV1::signalKinectError ,
            this,     &ART::slotKinectError);
    connect(kinectv1, &ARTKinectV1::signalDisplayLogMessage,
            this,     &ART::slotDisplayLogMessage);
    connect(this,     &ART::signalStartKinect,
            kinectv1, &ARTKinectV1::slotStartKinect);
    connect(this,     &ART::signalSetMinDistance,
            kinectv1, &ARTKinectV1::slotSetMinDistance);
    connect(this,     &ART::signalSetMaxDistance,
            kinectv1, &ARTKinectV1::slotSetMaxDistance);
    connect(kinectview, &ARTKinectWidget::signalSetOffsets,
            kinectv1,   &ARTKinectV1::slotSetOffsets);

    kinectthread->start();
    emit this->signalStartKinect();
  }
  else
  {
    kinectview->setKinectVersion(2);
    if(kinectv2 != NULL)
    {
      slotDisplayLogMessage("Kinect already running!");
      return;
    }
    slotDisplayLogMessage("Starting Kinect v2 ...");

    QMap<int, QString> mapping = loadBoneMapping(2);
    if(mapping.empty())
      return;

    kinectv2 = new ARTKinectV2(mapping, configuration->readSetting("iModMiddleware/FullBodyWithKinect").toBool());
    connect(frametimer, &QTimer::timeout,
            kinectv2,   &ARTKinectV2::slotSendFrame);

    kinectthread = new QThread();
    kinectv2->moveToThread(kinectthread);
    connect(kinectthread, &QThread::finished,
            kinectv2,     &QObject::deleteLater);
    connect(kinectv2,   &ARTKinectV2::signalDrawJoint,
            kinectview, &ARTKinectWidget::slotDrawJointV2);
    connect(kinectv2, &ARTKinectV2::signalKinectRunning ,
            this,     &ART::slotKinectStarted);
    connect(kinectv2, &ARTKinectV2::signalKinectStopped ,
            this,     &ART::slotKinectStopped);
    connect(kinectv2, &ARTKinectV2::signalKinectError ,
            this,     &ART::slotKinectError);
    connect(kinectv2, &ARTKinectV2::signalDisplayLogMessage,
            this,     &ART::slotDisplayLogMessage);
    connect(this,     &ART::signalStartKinect,
            kinectv2, &ARTKinectV2::slotStartKinect);
    connect(this,     &ART::signalSetMinDistance,
            kinectv2, &ARTKinectV2::slotSetMinDistance);
    connect(this,     &ART::signalSetMaxDistance,
            kinectv2, &ARTKinectV2::slotSetMaxDistance);
    connect(kinectview, &ARTKinectWidget::signalSetOffsets,
            kinectv2,   &ARTKinectV2::slotSetOffsets);

    kinectthread->start();
    emit this->signalStartKinect();
  }
#endif
}

void ART::on_tabWidget_currentChanged(int index)
{
#ifndef CONSOLE_MODE
  if(index == 0)
  {
    ui->frameratelabel->setText("Framerate (1 - 120):");
    ui->framerateSpinBox->setMinimum(1);
    ui->framerateSpinBox->setMaximum(120);
    if(ui->activeListWidget->count() > 0)
      ui->startButton->setEnabled(true);
    else
      ui->startButton->setEnabled(false);
  }
  else
  {
    ui->frameratelabel->setText("Framerate (1 - 30):");
    ui->framerateSpinBox->setMinimum(1);
    ui->framerateSpinBox->setMaximum(30);
    if(kinectv1 != NULL)
    {
#ifdef WIN32
      ui->startButton->setEnabled(kinectv1->isKinectRunning());
#endif
    }
    else
      ui->startButton->setEnabled(false);
  }
#endif
}

void ART::slotDisplayLogMessage(QString s)
{
#ifdef CONSOLE_MODE
  qDebug() << s;
#else
  ui->textBrowser->append(s);
#endif
}

void ART::on_startButton_clicked()
{
  if(artnetwork != NULL)
  {
#ifndef CONSOLE_MODE
    ui->textBrowser->append("ART network already running");
#endif
    return;
  }
#ifndef CONSOLE_MODE
  ui->startButton->setEnabled(false);
  ui->textBrowser->append("Starting ART network...");
  ui->oculusCalibrationButton->setEnabled(true);
  ui->checkBox->setEnabled(true);
  ui->filterRiftCheckBox->setEnabled(true);
  ui->oculusResetButton->setEnabled(true);
  ui->kinect_merge_data->setEnabled(true);
#endif

  QString tcpport, udpport;
  tcpport = readSetting("iModMiddleware/ARTSimulatedTCPPort");
  udpport = readSetting("iModMiddleware/ARTSimulatedUDPPort");
  if(tcpport.toInt()<1024 && udpport.toInt() < 1024)
  {
#ifdef CONSOLE_MODE
    qDebug() << "Error: Network ports not valid";
#else
    ui->textBrowser->append("Error: Network ports not valid");
#endif
    return;
  }

#ifdef CONSOLE_MODE
  artnetwork = new ARTNetwork(tcpport.toInt(), udpport.toInt(), false);
#else
  artnetwork = new ARTNetwork(tcpport.toInt(), udpport.toInt(), kinectv1 || kinectv2);
#endif
#ifndef CONSOLE_MODE
  artnetwork->setOculusDstPort(configuration->readSetting("iModMiddleware/OculusDestinationPort").toInt());
  artnetwork->setDefaultHeadtarget(configuration->readSetting("iModMiddleware/DefaultHeadtarget").toInt());
#endif
  networkthread = new QThread();
  artnetwork->moveToThread(networkthread);
  connect(networkthread, &QThread::started,
          artnetwork,    &ARTNetwork::runARTNetwork);
  connect(networkthread, &QThread::finished,
          artnetwork,    &QObject::deleteLater);
  connect(artnetwork,    &ARTNetwork::signalDisplayLogMessage,
          this,          &ART::slotDisplayLogMessage);
  connect(this,       &ART::signalAddClient,
          artnetwork, &ARTNetwork::slotAddClient);
  connect(this,       &ART::signalUpdateOculusCalibration,
          artnetwork, &ARTNetwork::slotUpdateOculusCalibration);
  connect(this,       &ART::signalUpdateOculusFilter,
          artnetwork, &ARTNetwork::slotUpdateOculusFilter);
  connect(this,       &ART::signalResetOculusCalibration,
          artnetwork, &ARTNetwork::slotResetOculusCalibration);
  connect(this,       &ART::signalReadOculusTracking,
          artnetwork, &ARTNetwork::slotReadOculusTracking);
  connect(this,       &ART::signalRemoveClient,
          artnetwork, &ARTNetwork::slotRemoveClient);
  connect(database,   &ARTDatabase::signalSendFrame,
          artnetwork, &ARTNetwork::slotSendARTData);
  connect(artnetwork, &ARTNetwork::signalSetConnectetClientsNumber,
          this,       &ART::slotSetClientNumber);
#ifdef WIN32
  if(kinectv1)
    connect(kinectv1,   &ARTKinectV1::signalSendARTData,
            artnetwork, &ARTNetwork::slotSendARTData);
  if(kinectv2)
    connect(kinectv2,   &ARTKinectV2::signalSendARTData,
            artnetwork, &ARTNetwork::slotSendARTData);
#endif
  networkthread->start();
#ifdef CONSOLE_MODE
  emit this->signalFillBuffer(cmtables.split("#", QString::SkipEmptyParts));
#else
  if(ui->tabWidget->currentIndex() == 0) //DBS
  {
    ui->kinectTab->setEnabled(false);
    QStringList tables;
    foreach(QListWidgetItem* i, ui->activeListWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
      tables.append(i->text());

    ui->progressBar->setVisible(true);
    emit this->signalFillBuffer(tables);
  }
  else
  {
    ui->databaseTab->setEnabled(false);
  }
#endif
  frametimer->start();

  // DEEEEEEEEEEBUG !
  // emit this->signalReadOculusTracking(configuration->readSetting("iModMiddleware/OculusTrackingSourcePort").toInt(), true);
}

QString ART::readSetting(QString value)
{
#ifdef CONSOLE_MODE
  QSettings* settingsfile;

  if(getenv("IMOD_ROOT") == NULL)
  {
    qDebug() << "IMOD_ROOT not set. Aborting";
    return "";
  }
  QDir configPath = QDir((QString::fromStdString(getenv("IMOD_ROOT")) + "/CPP/src/"));
  if(!QFileInfo(configPath, "configuration.ini").exists())
  {
    qDebug() << "Can't load configuration.ini from " + configPath.path() + ". Aborting.";
    return "";
  }
  settingsfile = new QSettings(configPath.path() + "/configuration.ini", QSettings::IniFormat, 0);

  return settingsfile->value(value, "").toString();
#else
  return configuration->readSetting(value).toString();
#endif
}

void ART::on_loadTabelsButton_clicked()
{
#ifndef CONSOLE_MODE
  ui->loadTabelsButton->setEnabled(false);
  ui->progressBar->setVisible(true);
#endif
  if(!database->isDatabaseOpen())
  {
#ifdef CONSOLE_MODE
    QCoreApplication::processEvents();
#else
    ui->textBrowser->append("Database not open: connecting...");
    QApplication::processEvents();
#endif
    emit this->signalConnectDatabase(readSetting("iModMiddleware/MySqlAdress"),
                                     readSetting("iModMiddleware/MySqlDatabase"),
                                     readSetting("iModMiddleware/MySqlUser"),
                                     readSetting("iModMiddleware/MySqlPassword"));
  }
#ifndef CONSOLE_MODE
  ui->textBrowser->append("Loading tables");
  emit this->signalGetAllTables();
#endif
}

void ART::on_AddTableButton_clicked()
{
#ifndef CONSOLE_MODE
  QList<QListWidgetItem*> tmp = ui->allTabelListWidget->selectedItems();
  for(int i = 0; i < tmp.length(); i++)
    ui->activeListWidget->addItem(tmp.at(i)->text());
  if(ui->activeListWidget->count() > 0)
  {
    ui->startButton->setEnabled(true);
    ui->acceptTableButton->setEnabled(true);
  }
  else
  {
    ui->startButton->setEnabled(false);
    ui->acceptTableButton->setEnabled(false);
  }
#endif
}

void ART::on_RemoveTableButton_clicked()
{
#ifndef CONSOLE_MODE
  QList<QListWidgetItem*> tmp = ui->activeListWidget->selectedItems();
  for(int i = 0; i < tmp.length(); i++)
    ui->activeListWidget->takeItem(ui->activeListWidget->row(tmp.at(i)));
  if(ui->activeListWidget->count() > 0)
  {
    ui->startButton->setEnabled(true);
    if(databasethread != NULL)
      ui->acceptTableButton->setEnabled(true);
  }
  else
  {
    ui->startButton->setEnabled(false);
    ui->acceptTableButton->setEnabled(false);
  }
#endif
}

void ART::on_TableUpButton_clicked()
{
#ifndef CONSOLE_MODE
  int currentIndex = ui->activeListWidget->currentRow();
  QListWidgetItem *currentItem = ui->activeListWidget->takeItem(currentIndex);
  ui->activeListWidget->insertItem(currentIndex-1, currentItem);
  ui->activeListWidget->setCurrentRow(currentIndex-1);
#endif
}

void ART::on_TableDownButton_clicked()
{
#ifndef CONSOLE_MODE
  int currentIndex = ui->activeListWidget->currentRow();
  if(currentIndex == -1)
    return;
  QListWidgetItem *currentItem = ui->activeListWidget->takeItem(currentIndex);
  ui->activeListWidget->insertItem(currentIndex+1, currentItem);
  ui->activeListWidget->setCurrentRow(currentIndex+1);
#endif
}

void ART::on_acceptTableButton_clicked()
{
#ifndef CONSOLE_MODE
  QStringList tables;
  foreach(QListWidgetItem* i, ui->activeListWidget->findItems(QString("*"), Qt::MatchWrap | Qt::MatchWildcard))
    tables.append(i->text());
  emit this->signalFillBuffer(tables);
  ui->progressBar->setVisible(true);
#endif
}

void ART::slotFilledBuffer(bool success)
{
#ifdef CONSOLE_MODE
  if(!success)
    qDebug() << "Errors occured while parsing tabels! Releasing buffer";
  else
    qDebug() << "Database ready";
#else
  if(!success)
  {
    qDebug() << "Errors occured while parsing tabels! Releasing buffer";
    ui->textBrowser->append("Errors occured while parsing tabels! Releasing buffer");
    this->sqlstate->setText("Database: Error");
  }
  else
  {
    ui->textBrowser->append("Database ready");
    ui->resetBuffer->setEnabled(true);
    ui->acceptTableButton->setEnabled(true);
    this->sqlstate->setText("Database: ready");
  }
  ui->progressBar->setVisible(false);
#endif
}

void ART::on_resetBuffer_clicked()
{
  emit this->signalResetBuffer();
}

void ART::on_framerateSpinBox_valueChanged(int arg1)
{
  frametimer->setInterval(1000/arg1);
}


void ART::on_kinectNearSlider_valueChanged(int value)
{
#ifndef CONSOLE_MODE
  if(value > ui->kinectFarSlider->value())
  {
    ui->kinectNearSlider->setValue(ui->kinectFarSlider->value());
    return;
  }
  ui->kinectNearLabel->setText(QString("%1").arg((value/100.0),0,'f',2)+ " m");
  emit this->signalSetMinDistance(value/100.0);
#endif
}

void ART::on_kinectFarSlider_valueChanged(int value)
{
#ifndef CONSOLE_MODE
  if(value < ui->kinectNearSlider->value())
  {
    ui->kinectFarSlider->setValue(ui->kinectNearSlider->value());
    return;
  }
  ui->kinectFarLabel->setText(QString("%1").arg((value/100.0),0,'f',2)+ " m");
  emit this->signalSetMaxDistance(value/100.0);
#endif
}

void ART::slotSetTableList(QStringList list)
{
#ifndef CONSOLE_MODE
  ui->allTabelListWidget->clear();

  if(list.contains("ARTData"))
    list.removeOne("ARTData");

  ui->allTabelListWidget->insertItems(0,list);
  ui->loadTabelsButton->setEnabled(true);
  ui->progressBar->setVisible(false);
#endif
}

void ART::slotSetClientNumber(int number)
{
#ifndef CONSOLE_MODE
  ui->connectedClientsLabel->setText(QString::number(number));
#endif
}

QMap<int, QString> ART::loadBoneMapping(int version)
{
 #ifndef CONSOLE_MODE
  QMap<int, QString> mapping;

  QString kinectgroupe = "ART_Kinect_V" + QString::number(version);
  QStringList artkeys = configuration->getGroupKeys(kinectgroupe);

  foreach(QString artkey, artkeys)
  {
    QString kinectenum = configuration->readSetting(kinectgroupe + "/" + artkey).toString();
    int     artid      = configuration->readSetting("ART_Targets/" + artkey).toInt();
    if(kinectenum != "NDef")
      mapping.insert(artid, kinectenum);
  }

  return mapping;
#endif
}

void ART::on_checkBox_stateChanged(int arg1)
{
#ifndef CONSOLE_MODE
  emit this->signalAddClient(-1, arg1 == 0 ? "" : readSetting("iModMiddleware/OculusDestinationIP"));
#endif
}

void ART::on_oculusCalibrationButton_clicked()
{
#ifndef CONSOLE_MODE
  emit this->signalUpdateOculusCalibration();
#endif
}

void ART::on_filterRiftCheckBox_stateChanged(int arg1)
{
#ifndef CONSOLE_MODE
  ui->oculus_filter_lineEdit->setEnabled(arg1 == 0 ? true : false);

  emit this->signalUpdateOculusFilter(ui->oculus_filter_lineEdit->text());
#endif
}

void ART::on_oculusResetButton_clicked()
{
  emit this->signalResetOculusCalibration();
}

void ART::on_connect_art_button_clicked()
{
#ifndef CONSOLE_MODE
  QString tcpport, udpport;
  tcpport = readSetting("iModMiddleware/ARTMiddlewareTCPPort");
  udpport = readSetting("iModMiddleware/ARTMiddlewareUDPPort");

  artnetwork = new ARTNetwork(tcpport.toInt(), udpport.toInt(), false);
  artnetwork->setOculusDstPort(configuration->readSetting("iModMiddleware/OculusDestinationPort").toInt());
  artnetwork->setDefaultHeadtarget(configuration->readSetting("iModMiddleware/DefaultHeadtarget").toInt());
  networkthread = new QThread();
  artnetwork->moveToThread(networkthread);
  connect(this,       &ART::signalConnectARTServer,
          artnetwork, &ARTNetwork::slotConnectARTServer);
  connect(artnetwork,    &ARTNetwork::signalDisplayLogMessage,
          this,          &ART::slotDisplayLogMessage);
  connect(networkthread, &QThread::started,
          artnetwork,    &ARTNetwork::runARTNetwork);
  connect(artnetwork, &ARTNetwork::signalArtFpsUpdate,
          this,       &ART::slotArtFpsUpdate);
  connect(this,       &ART::signalAddClient,
          artnetwork, &ARTNetwork::slotAddClient);

  connect(networkthread, &QThread::finished,
          artnetwork,    &QObject::deleteLater);
  connect(this,       &ART::signalUpdateOculusCalibration,
          artnetwork, &ARTNetwork::slotUpdateOculusCalibration);
  connect(this,       &ART::signalUpdateOculusFilter,
          artnetwork, &ARTNetwork::slotUpdateOculusFilter);
  connect(this,       &ART::signalResetOculusCalibration,
          artnetwork, &ARTNetwork::slotResetOculusCalibration);
  connect(this,       &ART::signalReadOculusTracking,
          artnetwork, &ARTNetwork::slotReadOculusTracking);
  connect(this,       &ART::signalRemoveClient,
          artnetwork, &ARTNetwork::slotRemoveClient);
  connect(database,   &ARTDatabase::signalSendFrame,
          artnetwork, &ARTNetwork::slotSendARTData);
  connect(artnetwork, &ARTNetwork::signalSetConnectetClientsNumber,
          this,       &ART::slotSetClientNumber);

  networkthread->start();

  ui->connect_art_button->setEnabled(false);
  emit this->signalConnectARTServer(configuration->readSetting("iModMiddleware/ARTServerUDPPort").toInt(),
                                    configuration->readSetting("iModMiddleware/ARTServerTCPPort").toInt(),
                                    configuration->readSetting("iModMiddleware/ARTServerAdress").toString());

  ui->oculusCalibrationButton->setEnabled(true);
  ui->checkBox->setEnabled(true);
  ui->filterRiftCheckBox->setEnabled(true);
  ui->oculusResetButton->setEnabled(true);
#endif
}

void ART::slotArtFpsUpdate(int fps)
{
#ifndef CONSOLE_MODE
  ui->artfps_label->setText("FPS: " + QString::number(fps));
#endif
}

void ART::on_kinect_merge_data_clicked(bool checked)
{
#ifndef CONSOLE_MODE
  emit this->signalReadOculusTracking(configuration->readSetting("iModMiddleware/OculusTrackingSourcePort").toInt(), checked);
#endif
}
