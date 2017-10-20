/**
 * @file View/imodview.h
 * @brief Implementation of the GUI
 */

#ifndef IMODVIEW_H
#define IMODVIEW_H

#include <QMainWindow>
#include <QtGui/QVector3D>

namespace Ui {
  class iModView;
}

#ifdef WIN64
typedef struct __declspec( align( 8 ) ) _rgo
#else
typedef struct __attribute__ ((aligned (8))) _rgo
#endif
{
    quint8 rACT;
    quint8 rMOD;
    quint8 rGTO;
    quint8 rATR;
    quint8 rGLV;
    quint8 rICF;
    quint8 rICS;
    quint8 rPRA;
    quint8 rSPA;
    quint8 rFRA;
    quint8 rPRB;
    quint8 rSPB;
    quint8 rFRB;
    quint8 rPRC;
    quint8 rSPC;
    quint8 rFRC;
    quint8 rPRS;
    quint8 rSPS;
    quint8 rFRS;
} RobotGripperOutput;


#ifdef WIN64
typedef struct __declspec( align( 8 ) ) _rgi
#else
typedef struct __attribute__ ((aligned (8))) _rgi
#endif
{
    quint8 gACT;
    quint8 gMOD;
    quint8 gGTO;
    quint8 gIMC;
    quint8 gSTA;
    quint8 gDTA;
    quint8 gDTB;
    quint8 gDTC;
    quint8 gDTS;
    quint8 gFLT;
    quint8 gPRA;
    quint8 gPOA;
    quint8 gCUA;
    quint8 gPRB;
    quint8 gPOB;
    quint8 gCUB;
    quint8 gPRC;
    quint8 gPOC;
    quint8 gCUC;
    quint8 gPRS;
    quint8 gPOS;
    quint8 gCUS;
} RobotGripperInput;


class iModKinectWidget;
class iModControl;
class QCheckBox;

/**
 * @class iModView
 * @brief The iModView class
 * @details This class provides the GUI for the Daemon. It contains no in depth logic. Basically it only communicates with the daemon over the iMod Control library.
 */
class iModView : public QMainWindow
{
    Q_OBJECT

  public:
    /**
     * @brief Constructor of iModView
     * @details Creates the controller, initializes the GUI and connects the required signals.
     * @param parent The parent of this class
     */
    explicit iModView(QWidget *parent = 0);
    /**
    * @brief Destructor of iModView
    */
    ~iModView();

  private slots:
    // GUI Elements
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_2_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_kinectStartButton_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_8_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_dbCategoryComboBox_currentTextChanged(const QString &arg1);
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_5_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_6_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_dbStartRecording_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_dbStopRecording_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_radioUseMiddleware_clicked(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_radioUseKinect_clicked(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_radioUseDatabase_clicked(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_dbReplaySpeed_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_10_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_tabWidget_currentChanged(int index);
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_11_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_checkBox_clicked(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_buttonforceroot_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_comboBox_clientlist_activated(const QString &arg1);
    /**
     * @brief Qt-GUI internals
     */
    void on_radioButton_clicked(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_radioButton_2_clicked(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_radioButton_3_clicked(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_KinectRangeMin_sliderMoved(int position);
    /**
     * @brief Qt-GUI internals
     */
    void on_KinectRangeMax_sliderMoved(int position);
    /**
     * @brief Qt-GUI internals
     */
    void on_ButtonKinectRange_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_KinectRangeMax_valueChanged(int value);
    /**
     * @brief Qt-GUI internals
     */
    void on_KinectRangeMin_valueChanged(int value);

    // Controller Stuff
    /**
     * @brief Recives privileges from the Daemom
     * @details If rc is zero, the client is now the root client and can control the daemon. Based on the rc value the GUI changes.
     * @param rc Current root client
     * @param rk Current root Kinect
     */
    void slotDaemonControll(quint8 rc, quint8 rk);
    /**
     * @brief Recives the control values for the robot
     * @param b Current blending
     * @param s Current speed
     * @param a Current speedup
     * @param en Sets, if the daemon is sending data to the Robot
     * @param outtype Sets the outputtype: Real robot, Real robot over ROS, File
     */
    void slotRobotControllValues(qreal b, qreal s, qreal a, bool en, char outtype);
    /**
     * @brief Sets the source of the tracking data used by the Daemon
     * @param value Source is selected based on this value: 0x00 = Real ART, 0x01 = Kinect, 0x02 = Database
     */
    void slotInputMethode(char value);
    /**
     * @brief Recives a list of all clients
     * @details This function updates the List of all clients and adds checkboxes in the Kinect-tab to set the near-/farclpipping plan
     * @param list Contains a list of all clientid currently in use
     */
    void slotDaemonState(QStringList list);
    /**
     * @brief Recives the connection state from Daemon<->Robot
     * @param con Connection state
     */
    void slotUR5Connection(bool con);
    /**
     * @brief Recives data for visualisation
     * @param bfps Current FPS Blender -> Daemon
     * @param rfps Current FPS Robot -> Daemon
     */
    void slotBlenderRobotFPS(int bfps, int rfps);
    /**
     * @brief Parsing and presenting the gripper status
     * @param data Gripper status
     */
    void slotGripperControlValues(QByteArray data);
    /**
     * @brief Updates the GUI, setting the two values
     * @param f Force
     * @param t Torque
     */
    void slotFTSensorValues(QVector3D f, QVector3D t);
    /**
     * @brief Qt-GUI internals
     */
    void on_pushButton_3_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_gripperMode_basic_toggled(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_gripperMode_pinch_toggled(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_gripperMode_wide_toggled(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_gripperMode_scissor_toggled(bool checked);
    /**
     * @brief Qt-GUI internals
     */
    void on_gripper_independentFingers_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_gripper_independentScissor_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_gripper_Activate_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_gripper_gloveMode_clicked();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerA_targetPosition_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerA_speed_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerA_force_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerB_targetPosition_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerB_speed_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerB_force_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerC_targetPosition_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerC_speed_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_fingerC_force_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_scissor_targetPosition_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_scissor_speed_editingFinished();
    /**
     * @brief Qt-GUI internals
     */
    void on_scissor_force_editingFinished();

    void on_pushButton_4_clicked();

    void on_radioButton_GC_A_FT_toggled(bool checked);

    void on_radioButton_GC_A_ML_toggled(bool checked);

    void on_radioButton_GC_M_toggled(bool checked);

  private:
    /**
     * @brief Sends Kinect offsets
     * @details This function took offsets from the Kinect-tab to transformt the merged user skeleton. The GUI-elements are hidden at the moment and the function is not used anymore.
     */
    void sendKinectOffsets();
    /**
     * @brief Sets the number of recognized ART-Targets
     * @details This thing is not used at the moment
     */
    void artTargetNumberUpdate();
    /**
     * @brief Creates the gripper status and sends it to the daemon
     */
    void createGripperStatus();

    /**
     * @brief Pointer to the user interface
     */
    Ui::iModView *ui;
    /**
     * @brief Instance of the controller
     */
    iModControl* controller;
    /**
     * @brief Holds the ID of the current rootkinect
     */
    quint8 rootkinect;
    /**
     * @brief Holds the current ID of this client
     */
    quint8 clientid;
    /**
     * @brief Holds a list of checkboxes in the Kinect-tab
     */
    QList<QCheckBox*> list_kinectcheckboxes;
};

#endif // IMODVIEW_H
