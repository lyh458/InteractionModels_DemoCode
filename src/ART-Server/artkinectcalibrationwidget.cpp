#include "artkinectcalibrationwidget.h"
#include "ui_artkinectcalibrationwidget.h"

ARTKinectCalibrationWidget::ARTKinectCalibrationWidget(QWidget *parent) :
  QDialog(parent),
  ui(new Ui::ARTKinectCalibrationWidget)
{
  ui->setupUi(this);
  ui->x_loc->setText(getValueString(0, "m"));
  ui->y_loc->setText(getValueString(0, "m"));
  ui->z_loc->setText(getValueString(0, "m"));
  ui->x_rot->setText("0 °");
  ui->y_rot->setText("0 °");
  ui->z_rot->setText("0 °");
  x_loc = 0;
  y_loc = 0;
  z_loc = 0;
  x_rot = 0;
  y_rot = 0;
  z_rot = 0;
}

ARTKinectCalibrationWidget::~ARTKinectCalibrationWidget()
{
  delete ui;
}

QString ARTKinectCalibrationWidget::getValueString(int value, QString extension)
{
  return QString("%1").arg((value/100.0),0,'f',2)+ " " + extension;
}


void ARTKinectCalibrationWidget::on_x_loc_slider_valueChanged(int value)
{
  ui->x_loc->setText(getValueString(value, "m"));
  x_loc = value;
  calibrationValuesChanged();
}

void ARTKinectCalibrationWidget::on_y_loc_slider_valueChanged(int value)
{
  ui->y_loc->setText(getValueString(value, "m"));
  y_loc = value;
  calibrationValuesChanged();
}

void ARTKinectCalibrationWidget::on_z_loc_slider_valueChanged(int value)
{
  ui->z_loc->setText(getValueString(value, "m"));
  z_loc = value;
  calibrationValuesChanged();
}

void ARTKinectCalibrationWidget::on_x_rot_dial_valueChanged(int value)
{
  ui->x_rot->setText(QString::number(value) + " °");
  x_rot = value;
  calibrationValuesChanged();
}

void ARTKinectCalibrationWidget::on_y_rot_dial_valueChanged(int value)
{
  ui->y_rot->setText(QString::number(value) + " °");
  y_rot = value;
  calibrationValuesChanged();
}

void ARTKinectCalibrationWidget::on_z_rot_dial_valueChanged(int value)
{
  ui->z_rot->setText(QString::number(value) + " °");
  z_rot = value;
  calibrationValuesChanged();
}

void ARTKinectCalibrationWidget::calibrationValuesChanged()
{
  emit this->signalSetOffsets(x_loc/100.0, y_loc/100.0, z_loc/100.0, x_rot, y_rot, z_rot);
}
