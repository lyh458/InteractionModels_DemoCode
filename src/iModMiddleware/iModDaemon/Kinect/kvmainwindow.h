/**
* @file iModDaemon/Kinect/kvmainwindow.h
* @brief This file contains the class for multiple kinect views.
*/

#ifndef KVMAINWINDOW_H
#define KVMAINWINDOW_H

#include <QtWidgets/QWidget>
#include <QtCore/QMap>
#include <QtGui/QMatrix4x4>

#include "imodkinecttypes.h"

class KVKinect;
/**
 * @class KVMainWindow
 * @brief This class manages multiple instances of KVKinect
 * @details This class is designed to show the skeletons of three Kinects in addition to the merged view. It's recommendet to evaluate the tracking with only one user inside the tracking volume.
 */
class KVMainWindow : public QWidget
{
    Q_OBJECT
  public:
    /**
     * @brief Constructor of KVMainWindow
     * @param parent Parent of this window (parent = NULL for standallone window)
     */
    explicit KVMainWindow(QWidget *parent = 0);

  signals:

  public slots:
    /**
     * @brief Updates the drawing of a certail Kinect skeleton
     * @details The lists are serialized, becaus the signal-slot-system cant pass QList<type> without further metatype registration.
     * @param kid Kinectid of the certain view.
     * @param s_data 3D jointdata serialized as QByteArray
     * @param s_tstate Trackingstates of joints serialized as QByteArray.
     */
    void slotUpdateDrawing(int kid, QByteArray s_data, QByteArray s_tstate);
    /**
     * @brief Sets the transitions of the 3D data.
     * @param _kt Kinecttransitions holding the transitions used for each Kinect to transforme their data to the root kinect system.
     */
    void slotSetTransitions(QMap<int, QMatrix4x4> _kt);

  private:
    /**
     * @brief Gets the next free view that isnt used by a kinect by now.
     * @return Returns the next free view id, if a view is avaible
     */
    int getNextFreeView();
    template<typename type>
    /**
     * @brief Deserializes a QByteArray into a QList of a certain type.
     * @param data Byte array with serialized QList inside.
     * @return Returns a list with the desired type.
     */
    QList<type> deserializeQList(QByteArray data)
    {
      QList<type> result;
      type dummy;
      int datacount = data.count() / sizeof(type);
      for(int i = 0; i < datacount; i++)
      {
        memcpy(&dummy, data.data() + i * sizeof(type), sizeof(type));
        result.append(dummy);
      }
      return result;
    }

    /**
     * @brief Holds a list of all views.
     */
    QList<KVKinect*> view_kinectlist;
    /**
     * @brief Holds a pointer to the view that shows the merged results. This is the last element of view_kinectlist.
     */
    KVKinect* view_merged;
    /**
     * @brief Maps a kinectID to a view number.
     */
    QMap<int, int> view_usage;
    /**
     * @brief Maps the kinectID to the corresponding transitions.
     */
    QMap<int, QMatrix4x4> kinecttransitions;
};

#endif // KVMAINWINDOW_H
