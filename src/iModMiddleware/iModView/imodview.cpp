#include "imodview.h"
#include "ui_imodview.h"

#include "imodcontrol.h"
#include <QtWidgets/QMessageBox>
#include <QtCore/QDebug>
#include <QtWidgets/QInputDialog>
#include <QtGui/QVector3D>
#include <QtWidgets/QCheckBox>
#include <QtWidgets/QLayout>

iModView::iModView(QWidget *parent) :
  QMainWindow(parent),
  ui(new Ui::iModView)
{
  ui->setupUi(this);
  ui->tabWidget->setEnabled(false);
  ui->ipPortEdit->setValidator(new QRegExpValidator(QRegExp("[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}\\.[0-9]{1,3}"), this));

  controller = new iModControl();
  controller->connectDaemon();

  connect(controller, &iModControl::signalDaemonControll,      this, &iModView::slotDaemonControll      );
  connect(controller, &iModControl::signalRobotControllValues, this, &iModView::slotRobotControllValues );
  connect(controller, &iModControl::signalInputMethode,        this, &iModView::slotInputMethode        );
  connect(controller, &iModControl::signalDaemonState,         this, &iModView::slotDaemonState         );
  connect(controller, &iModControl::signalUR5Connection,       this, &iModView::slotUR5Connection       );
  connect(controller, &iModControl::signalBlenderRobotFPS,     this, &iModView::slotBlenderRobotFPS     );
  connect(controller, &iModControl::signalGripperControlValue, this, &iModView::slotGripperControlValues);
  connect(controller, &iModControl::signalFTSensorValues,      this, &iModView::slotFTSensorValues      );

  ui->dbCategoryComboBox->addItem("<< New Category >>");
#ifndef WIN64
  ui->kinectStartButton->setEnabled(false);
#endif
  ui->dbStopRecording->setEnabled(false);
}

iModView::~iModView()
{
  delete ui;
}

void iModView::on_pushButton_clicked()
{
  controller->connectUR5(ui->ipPortEdit->text(), 30003);
}

void iModView::on_pushButton_2_clicked()
{
  controller->resetDaemon();
}

void iModView::on_kinectStartButton_clicked()
{
#ifdef WIN64
  controller->startKinect(clientid);
  sendKinectOffsets();
#endif
}

void iModView::sendKinectOffsets()
{
  // Depricated
}

void iModView::on_pushButton_8_clicked()
{
  QStringList values;
  controller->getDatabaseCategorylist(values);
  ui->dbCategoryComboBox->clear();
  ui->dbCategoryComboBox->insertItems(0, values);
  ui->dbCategoryComboBox->addItem("<< New Category >>");
}

void iModView::on_dbCategoryComboBox_currentTextChanged(const QString &arg1)
{
  if(arg1 != "<< New Category >>")
  {
    QStringList values;
    controller->getDatabaseTableList(values, arg1);
    ui->dbTablesComboBox->clear();
    ui->dbTablesComboBox->insertItems(0, values);
  }
  else if(arg1 == "<< New Category >>" && ui->dbCategoryComboBox->count() > 1)
  {
    bool ok;
    QString text = QInputDialog::getText(this, tr("QInputDialog::getText()"),
                                         tr("Recording name:"), QLineEdit::Normal,
                                         "", &ok);
    if (ok && !text.isEmpty())
    {
      ui->dbCategoryComboBox->addItem(text);
      ui->dbCategoryComboBox->setCurrentIndex(ui->dbCategoryComboBox->findText(text));
    }
  }

}

void iModView::on_pushButton_5_clicked()
{
  controller->startARTTracking(true);
}

void iModView::on_pushButton_6_clicked()
{
  controller->startARTTracking(false);
}

void iModView::on_dbStartRecording_clicked()
{
  ui->dbStartRecording->setEnabled(false);
  ui->dbStopRecording->setEnabled(true);
  controller->startDatabaseRecording(true, ui->dbReplaySpeed->text().toUInt());
  ui->dbRecordingStatus->setText("RECORDING");
  ui->dbRecordingStatus->setStyleSheet("QLabel { background-color : green; }");
}

void iModView::on_dbStopRecording_clicked()
{
  ui->dbStartRecording->setEnabled(true);
  ui->dbStopRecording->setEnabled(false);
  controller->startDatabaseRecording(false, ui->dbReplaySpeed->text().toUInt());
  controller->writeRecordingToDatabase(ui->dbCategoryComboBox->currentText(), ui->dbNewTableName->text());
  ui->dbRecordingStatus->setText("NOT RECORDING");
  ui->dbRecordingStatus->setStyleSheet("QLabel { background-color : red; }");
}

void iModView::on_radioUseMiddleware_clicked(bool checked)
{
  if(checked)
    controller->setInputMethode(0x00);
}

void iModView::on_radioUseKinect_clicked(bool checked)
{
  if(checked)
    controller->setInputMethode(0x01);
}

void iModView::on_radioUseDatabase_clicked(bool checked)
{
  if(checked)
    controller->setInputMethode(0x02);
}

void iModView::on_dbReplaySpeed_editingFinished()
{
  controller->setDatabaseReplayFPS(ui->dbReplaySpeed->text().toInt());
}

void iModView::on_pushButton_10_clicked()
{
  controller->loadCurrentTable(ui->dbCategoryComboBox->currentText(), ui->dbTablesComboBox->currentText());
}

void iModView::on_tabWidget_currentChanged(int index)
{
  // Sending some state requests
  if(ui->tabWidget->tabText(index) == "ART")
  {

  }
}

void iModView::on_pushButton_11_clicked()
{
  controller->setRobotControllValues(ui->ur5BlendingEdit->text().toDouble(),
                                     ui->ur5MaxSpeedEdit->text().toDouble(),
                                     ui->ur5MaxSpeedupEdit->text().toDouble(),
                                     ui->checkBox->checkState() == Qt::Checked);
}

void iModView::on_checkBox_clicked(bool checked)
{
  Q_UNUSED(checked);
  on_pushButton_11_clicked();
}

void iModView::artTargetNumberUpdate()
{
  quint8 targets;
  controller->getIdentifiedTargets(targets);
  ui->artTargetNumberLabel->setText("Current ART Targets: " + QString::number(targets));
}

void iModView::createGripperStatus()
{
  RobotGripperOutput go;
  go.rACT = 0x01;
  go.rATR = 0x00;
  go.rFRA = ui->fingerA_force->value();
  go.rFRB = ui->fingerB_force->value();
  go.rFRC = ui->fingerC_force->value();
  go.rFRS = ui->scissor_force->value();
  go.rGLV = ui->gripper_gloveMode->isChecked()?0x01:0x00;
  go.rGTO = ui->gripper_Activate->isChecked()?0x01:0x00;
  go.rICF = ui->gripper_independentFingers->isChecked()?0x01:0x00;
  go.rICS = ui->gripper_independentScissor->isChecked()?0x01:0x00;
  go.rMOD = ui->gripperMode_basic->isChecked()?0x00:ui->gripperMode_pinch->isChecked()?0x01:ui->gripperMode_wide->isChecked()?0x02:0x03;
  go.rPRA = ui->fingerA_targetPosition->value();
  go.rPRB = ui->fingerB_targetPosition->value();
  go.rPRC = ui->fingerC_targetPosition->value();
  go.rPRS = ui->scissor_targetPosition->value();
  go.rSPA = ui->fingerA_speed->value();
  go.rSPB = ui->fingerB_speed->value();
  go.rSPC = ui->fingerC_speed->value();
  go.rSPS = ui->scissor_speed->value();

  QByteArray status;
  status.resize(sizeof(RobotGripperOutput));
  memcpy(status.data(), &go, sizeof(RobotGripperOutput));
  controller->setGripperStatus(status);
}

void iModView::on_buttonforceroot_clicked()
{
  controller->forceRoot();
}

void iModView::on_comboBox_clientlist_activated(const QString &arg1)
{
  int id = arg1.toInt();
  controller->setRootKinect(id);
}

void iModView::slotDaemonControll(quint8 rc, quint8 rk)
{
  rootkinect = rk;
  clientid = rc;
  if(ui->tabWidget->isEnabled() == false) // first start
  {
    // Make some init stuff
    ui->tabWidget->setEnabled(true);
  }
  if(rc == 0)
  {
    ui->ur5Tab->setEnabled(true);
    ui->artTab->setEnabled(true);
    ui->databaseTab->setEnabled(true);
    ui->ur5GripperTab->setEnabled(true);
    ui->pushButton_2->setEnabled(false); // disabled the thing...
    ui->comboBox_clientlist->setEnabled(true);
    ui->KinectRangeMax->setEnabled(true);
    ui->KinectRangeMin->setEnabled(true);
    ui->ButtonKinectRange->setEnabled(true);
  }
  else
  {
    ui->ur5Tab->setEnabled(false);
    ui->artTab->setEnabled(false);
    ui->databaseTab->setEnabled(false);
    ui->ur5GripperTab->setEnabled(false);
    ui->pushButton_2->setEnabled(false);
    ui->comboBox_clientlist->setEnabled(false);
    ui->KinectRangeMax->setEnabled(false);
    ui->KinectRangeMin->setEnabled(false);
    ui->ButtonKinectRange->setEnabled(false);
  }
}

void iModView::slotUR5Connection(bool con)
{
  if(con)
    ui->ur5StateLabel->setText("Connected");
  else
    ui->ur5StateLabel->setText("Cant connect");
}

void iModView::slotBlenderRobotFPS(int bfps, int rfps)
{
  ui->robotFPSLabel->setText("Robot FPS:\t" + QString::number(rfps));
  ui->blenderFPSLabel->setText("Blender FPS:\t" + QString::number(bfps));
}

void iModView::slotGripperControlValues(QByteArray data)
{
  if(sizeof(RobotGripperInput) != data.size())
    qDebug() << "Gripper status parsing error";

  RobotGripperInput gi;
  memcpy(&gi, data.data(), sizeof(RobotGripperInput));

  ui->label_gripperConnected->setText("Connected");
  ui->fingerA_current->setText(QString::number(gi.gCUA * 0.1, 'f', 4));
  ui->fingerB_current->setText(QString::number(gi.gCUB * 0.1, 'f', 4));
  ui->fingerC_current->setText(QString::number(gi.gCUC * 0.1, 'f', 4));
  ui->scissor_current->setText(QString::number(gi.gCUS * 0.1, 'f', 4));
  ui->fingerA_currentPosition->setText(QString::number(gi.gPOA));
  ui->fingerB_currentPosition->setText(QString::number(gi.gPOB));
  ui->fingerC_currentPosition->setText(QString::number(gi.gPOC));
  ui->scissor_currentPosition->setText(QString::number(gi.gPOS));

  switch(gi.gFLT)
  {
    case 0x00:
      ui->label_gripperError->setText("Status: OK");
      break;
    case 0x05:
      ui->label_gripperError->setText("Status: Action delayed, activation must be completed");
      break;
    case 0x06:
      ui->label_gripperError->setText("Status: Action delayed, mode change must be completed");
      break;
    case 0x07:
      ui->label_gripperError->setText("Status: The activation bit must be set prior to action");
      break;
    case 0x09:
      ui->label_gripperError->setText("Status: The communication chip is not ready");
      break;
    case 0x0A:
      ui->label_gripperError->setText("Status: Changing mode fault, interference detected on Scissor");
      break;
    case 0x0B:
      ui->label_gripperError->setText("Status: Automatic release in progress");
      break;
    case 0x0D:
      ui->label_gripperError->setText("Status: Activation fault (interference or other error?)");
      break;
    case 0x0E:
      ui->label_gripperError->setText("Status: Changing mode fault, interference detected on Scissor");
      break;
    case 0x0F:
      ui->label_gripperError->setText("Status: Reset and activation is required");
      break;
    default:
      ui->label_gripperError->setText("Status: Unknown");
      break;
  }
  QPalette p;
  switch(gi.gDTA)
  {
    case 0x00:
      p.setColor(QPalette::WindowText, QColor("#FF6B00"));
      break;
    case 0x01:
    case 0x02:
      p.setColor(QPalette::WindowText, QColor("#E90027"));
      break;
    case 0x03:
      p.setColor(QPalette::WindowText, QColor("#75BA20"));
  }
  ui->label_fingerA->setPalette(p);
  switch(gi.gDTB)
  {
    case 0x00:
      p.setColor(QPalette::WindowText, QColor("#FF6B00"));
      break;
    case 0x01:
    case 0x02:
      p.setColor(QPalette::WindowText, QColor("#E90027"));
      break;
    case 0x03:
      p.setColor(QPalette::WindowText, QColor("#75BA20"));
  }
  ui->label_fingerB->setPalette(p);
  switch(gi.gDTC)
  {
    case 0x00:
      p.setColor(QPalette::WindowText, QColor("#FF6B00"));
      break;
    case 0x01:
    case 0x02:
      p.setColor(QPalette::WindowText, QColor("#E90027"));
      break;
    case 0x03:
      p.setColor(QPalette::WindowText, QColor("#75BA20"));
  }
  ui->label_fingerC->setPalette(p);
  switch(gi.gDTS)
  {
    case 0x00:
      p.setColor(QPalette::WindowText, QColor("#FF6B00"));
      break;
    case 0x01:
    case 0x02:
      p.setColor(QPalette::WindowText, QColor("#E90027"));
      break;
    case 0x03:
      p.setColor(QPalette::WindowText, QColor("#75BA20"));
  }
  ui->label_scissor->setPalette(p);
}

void iModView::slotFTSensorValues(QVector3D f, QVector3D t)
{
  ui->lineEdit_FT_FX->setText(QString::number(f.x(), 'f', 4));
  ui->lineEdit_FT_FY->setText(QString::number(f.y(), 'f', 4));
  ui->lineEdit_FT_FZ->setText(QString::number(f.z(), 'f', 4));
  ui->lineEdit_FT_TX->setText(QString::number(t.x(), 'f', 4));
  ui->lineEdit_FT_TY->setText(QString::number(t.y(), 'f', 4));
  ui->lineEdit_FT_TZ->setText(QString::number(t.z(), 'f', 4));
}

void iModView::slotRobotControllValues(qreal b, qreal s, qreal a, bool en, char outtype)
{
  ui->ur5BlendingLabel->setText("Blending (" + QString::number(b, 'f', 4) + ")");
  ui->ur5MaxSpeedLabel->setText("Max Speed  (" + QString::number(s, 'f', 4) + ")");
  ui->ur5MaxSpeedupLabel->setText("Max Speedup (" + QString::number(a, 'f', 4) + ")");
  ui->checkBox->setChecked(en);

  ui->radioButton->setChecked(false);
  ui->radioButton_2->setChecked(false);
  ui->radioButton_3->setChecked(false);

  if(outtype == 0x00)
    ui->radioButton->setChecked(true);
  else if(outtype == 0x01)
    ui->radioButton_2->setChecked(true);
  else
    ui->radioButton_3->setChecked(true);
}

void iModView::slotInputMethode(char value)
{
  ui->radioUseMiddleware->setChecked(value == 0x00);
  ui->radioUseKinect->setChecked(value == 0x01);
  ui->radioUseDatabase->setChecked(value == 0x02);
}

void iModView::slotDaemonState(QStringList list)
{
  ui->comboBox_clientlist->clear();
  ui->comboBox_clientlist->addItems(list);
  ui->comboBox_clientlist->setCurrentText(QString::number(rootkinect));

  for(int i = 0; i < list_kinectcheckboxes.count(); i++)
  {
    this->layout()->removeWidget(list_kinectcheckboxes[i]);
    delete list_kinectcheckboxes[i];
  }
  list_kinectcheckboxes.clear();

  for(int i = 0; i < list.count(); i++)
  {
    QCheckBox* cb = new QCheckBox(ui->kinectTab);
    cb->setText(list[i]);
    cb->move(120 + i * 60, 100);
    cb->show();
    list_kinectcheckboxes.append(cb);
  }
}

void iModView::on_radioButton_clicked(bool checked)
{
  if(checked)
    controller->setUR5OutputMode(0x00);
}

void iModView::on_radioButton_2_clicked(bool checked)
{
  if(checked)
    controller->setUR5OutputMode(0x01);
}

void iModView::on_radioButton_3_clicked(bool checked)
{
  if(checked)
    controller->setUR5OutputMode(0x02);
}

void iModView::on_KinectRangeMin_sliderMoved(int position)
{
  if(position >= ui->KinectRangeMax->value())
  {
    ui->KinectRangeMin->setValue(ui->KinectRangeMax->value());
  }
  ui->Label_KinectMin->setText(QString::number(ui->KinectRangeMin->value() / 100.0, 'f', 2) + " m");
}

void iModView::on_KinectRangeMax_sliderMoved(int position)
{
  if(position <= ui->KinectRangeMin->value())
  {
    ui->KinectRangeMax->setValue(ui->KinectRangeMin->value());
  }
  ui->Label_KinectMax->setText(QString::number(ui->KinectRangeMax->value() / 100.0, 'f', 2) + " m");
}

void iModView::on_ButtonKinectRange_clicked()
{
  QList<int> clients;
  for(int i = 0; i < list_kinectcheckboxes.count(); i++)
    if(list_kinectcheckboxes[i]->isChecked())
      clients.append(QString(list_kinectcheckboxes[i]->text()).toInt());

  controller->setKinectMinMaxRange(clients, ui->KinectRangeMin->value() / 100.0, ui->KinectRangeMax->value() / 100.0);
}

void iModView::on_KinectRangeMax_valueChanged(int value)
{
  on_KinectRangeMax_sliderMoved(value);
}

void iModView::on_KinectRangeMin_valueChanged(int value)
{
  on_KinectRangeMin_sliderMoved(value);
}

void iModView::on_pushButton_3_clicked()
{
  controller->connectGripper(ui->lineEdit_gripperIP->text());
}

void iModView::on_gripperMode_basic_toggled(bool checked)
{
  if(checked) createGripperStatus();
}

void iModView::on_gripperMode_pinch_toggled(bool checked)
{
  if(checked) createGripperStatus();
}

void iModView::on_gripperMode_wide_toggled(bool checked)
{
  if(checked) createGripperStatus();
}

void iModView::on_gripperMode_scissor_toggled(bool checked)
{
  if(checked) createGripperStatus();
}

void iModView::on_gripper_independentFingers_clicked()
{
  createGripperStatus();
}

void iModView::on_gripper_independentScissor_clicked()
{
  createGripperStatus();
}

void iModView::on_gripper_Activate_clicked()
{
  createGripperStatus();
}

void iModView::on_gripper_gloveMode_clicked()
{
  createGripperStatus();
}

void iModView::on_fingerA_targetPosition_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerA_speed_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerA_force_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerB_targetPosition_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerB_speed_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerB_force_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerC_targetPosition_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerC_speed_editingFinished()
{
  createGripperStatus();
}

void iModView::on_fingerC_force_editingFinished()
{
  createGripperStatus();
}

void iModView::on_scissor_targetPosition_editingFinished()
{
  createGripperStatus();
}

void iModView::on_scissor_speed_editingFinished()
{
  createGripperStatus();
}

void iModView::on_scissor_force_editingFinished()
{
  createGripperStatus();
}

void iModView::on_pushButton_4_clicked()
{
  controller->setOpenCVValues(QVector3D(ui->lineEdit_od_min_h->text().toShort(),
                                        ui->lineEdit_od_min_s->text().toShort(),
                                        ui->lineEdit_od_min_v->text().toShort()),
                              QVector3D(ui->lineEdit_od_max_h->text().toShort(),
                                        ui->lineEdit_od_max_s->text().toShort(),
                                        ui->lineEdit_od_max_v->text().toShort()),
                              ui->lineEdit_od_erode->text().toShort(),
                              ui->lineEdit_od_delate->text().toShort(),
                              ui->checkBox_od_show->isChecked());
}

void iModView::on_radioButton_GC_A_FT_toggled(bool checked)
{
  if(checked)
    controller->setGripperAutomatic(2);
}

void iModView::on_radioButton_GC_A_ML_toggled(bool checked)
{
  if(checked)
    controller->setGripperAutomatic(1);
}

void iModView::on_radioButton_GC_M_toggled(bool checked)
{
  if(checked)
    controller->setGripperAutomatic(0);
}
