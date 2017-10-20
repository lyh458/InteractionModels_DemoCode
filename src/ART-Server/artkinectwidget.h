#ifdef WIN32
#ifndef ARTKINECTWIDGET_H
#define ARTKINECTWIDGET_H

#include <QtWidgets/QWidget>

class ARTKinectCalibrationWidget;
class ARTKinectWidget : public QWidget
{
    Q_OBJECT
  typedef struct _joint
  {
    QPoint pos;
    bool tracked;
    bool calculated;
  }joint;

  public:
    ARTKinectWidget(QWidget *parent, bool fullbody);

    void setDisplayText(QString tmp) { displaytext = tmp; }
    void setKinectVersion(int i);

  protected:
    virtual void paintEvent(QPaintEvent *event);
    virtual void mousePressEvent(QMouseEvent * event);

  private:
    void drawBone(QPainter* p, joint joint1, joint joint2);

  signals:
    void signalStartKinect();
    void signalSetOffsets(double, double, double, int, int, int);

  public slots:
    void slotDrawJointV1(int i, int x, int y, bool tracked);
    void slotDrawJointV2(int i, int x, int y, bool tracked, bool calculated);

  private slots:
    void slotCalibrationFinished(int result);
    void slotSetOffsets(double x_loc, double y_loc, double z_loc, int x_rot, int y_rot, int z_rot)
    { emit this->signalSetOffsets(x_loc, y_loc, z_loc, x_rot, y_rot, z_rot); }

  private:
    QVector<joint>* jointlist;
    QString displaytext;
    int kinectversion;
    ARTKinectCalibrationWidget* calibrationwidget;
    bool fullbodytracking;

};

#endif // ARTKINECTOGLWIDGET_H
#endif
