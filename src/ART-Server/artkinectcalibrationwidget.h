#ifndef ARTKINECTCALIBRATIONWIDGET_H
#define ARTKINECTCALIBRATIONWIDGET_H

#include <QDialog>

namespace Ui {
  class ARTKinectCalibrationWidget;
}

class ARTKinectCalibrationWidget : public QDialog
{
    Q_OBJECT

  public:
    explicit ARTKinectCalibrationWidget(QWidget *parent = 0);
    ~ARTKinectCalibrationWidget();

  private:
    void calibrationValuesChanged();

  private slots:
    void on_x_loc_slider_valueChanged(int value);
    void on_y_loc_slider_valueChanged(int value);
    void on_z_loc_slider_valueChanged(int value);
    void on_x_rot_dial_valueChanged(int value);
    void on_y_rot_dial_valueChanged(int value);
    void on_z_rot_dial_valueChanged(int value);

  signals:
    void signalSetOffsets(double, double, double, int, int, int);

  private:
    QString getValueString(int value, QString extension);

  private:
    Ui::ARTKinectCalibrationWidget *ui;
    int x_loc;
    int y_loc;
    int z_loc;
    int x_rot;
    int y_rot;
    int z_rot;
};

#endif // ARTKINECTCALIBRATIONWIDGET_H
