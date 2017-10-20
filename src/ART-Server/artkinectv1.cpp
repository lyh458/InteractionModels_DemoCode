#ifdef WIN32
#include "artkinectv1.h"

#include <QtCore/QDebug>
#include <QtWidgets/QApplication>
#include <QtMath>

#include "artkinectwidget.h"


//---------------------------------------------------------------------------------------------------------------------
// Constructor
//---------------------------------------------------------------------------------------------------------------------
ARTKinectV1::ARTKinectV1(QMap<int, QString> bonemapping, bool fullbody) :
  ARTKinectBase(bonemapping, fullbody),
  m_hNextSkeletonEvent(INVALID_HANDLE_VALUE),
  m_pNuiSensor(NULL)
{
  fullbodytracking = false;
}
//---------------------------------------------------------------------------------------------------------------------
// Desstructor
//---------------------------------------------------------------------------------------------------------------------
ARTKinectV1::~ARTKinectV1()
{
  if (m_pNuiSensor)
    m_pNuiSensor->NuiShutdown();

  if (m_hNextSkeletonEvent && (m_hNextSkeletonEvent != INVALID_HANDLE_VALUE))
    CloseHandle(m_hNextSkeletonEvent);
}
//---------------------------------------------------------------------------------------------------------------------
// Starting kinect SDK and surching for some usable kinect's
//---------------------------------------------------------------------------------------------------------------------
HRESULT ARTKinectV1::startKinect()
{
  QMutexLocker locker(&kinectMutex);

  int iSensorCount = 0;
  HRESULT hr = NuiGetSensorCount(&iSensorCount);
  INuiSensor * pNuiSensor;

  if (FAILED(hr))
  {
    return hr;
  }

  for (int i = 0; i < iSensorCount; ++i)
  {
    // Create the sensor so we can check status, if we can't create it, move on to the next
    hr = NuiCreateSensorByIndex(i, &pNuiSensor);
    if (FAILED(hr))
      continue;

    // Get the status of the sensor, and if connected, then we can initialize it
    hr = pNuiSensor->NuiStatus();
    if (S_OK == hr)
    {
      m_pNuiSensor = pNuiSensor;
      break;
    }

    // This sensor wasn't OK, so release it since we're not using it
    pNuiSensor->Release();
  }

  if (NULL != m_pNuiSensor)
  {
    // Initialize the Kinect and specify that we'll be using skeleton
    hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
    if (SUCCEEDED(hr))
    {
      // Create an event that will be signaled when skeleton data is available
      m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);
      // Open a skeleton stream to receive skeleton data
      hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);
    }
  }

//  if(m_pNuiSensor != NULL)
//    m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, NUI_SKELETON_TRACKING_FLAG_ENABLE_SEATED_SUPPORT);

  if (NULL == m_pNuiSensor || FAILED(hr))
  {
    emit this->signalKinectError("No ready Kinect found!");
    return E_FAIL;
  }

  emit this->signalKinectRunning();
  runkinect = true;

  return hr;
}
//---------------------------------------------------------------------------------------------------------------------
// Updatemethode for kinect...
//---------------------------------------------------------------------------------------------------------------------
void ARTKinectV1::update()
{
  QMutexLocker locker(&kinectMutex);

  if (m_pNuiSensor == NULL)
    return;

  if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
  {
    processSkeleton();
  }
}
//---------------------------------------------------------------------------------------------------------------------
// ...processing skeleton data in event loop
//---------------------------------------------------------------------------------------------------------------------
void ARTKinectV1::processSkeleton()
{
  //QMutexLocker locker(&kinectMutex); Only called from update->locked

  NUI_SKELETON_FRAME skeletonFrame = {0};

  HRESULT hr = m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame);
  if ( FAILED(hr) )
    return;

  // smooth out the skeleton data
  m_pNuiSensor->NuiTransformSmooth(&skeletonFrame, NULL);

  // Endure Direct2D is ready to draw
  // This ist 5 because we can handel 5 sceletons....
  bool map = true;
  for (int i = 0 ; i < NUI_SKELETON_COUNT && map; ++i)
  {
    NUI_SKELETON_TRACKING_STATE trackingState = skeletonFrame.SkeletonData[i].eTrackingState;

    if (NUI_SKELETON_TRACKED == trackingState &&
        skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].z < maxdistance &&
        skeletonFrame.SkeletonData[i].SkeletonPositions[NUI_SKELETON_POSITION_SHOULDER_CENTER].z > mindistance )
    {
      map = false;
      // We're tracking the skeleton, handle it
      for (int j = 0; j < NUI_SKELETON_POSITION_COUNT; ++j)
      {
        LONG x,y;
        USHORT d;
        NuiTransformSkeletonToDepthImage(skeletonFrame.SkeletonData[i].SkeletonPositions[j], &x, &y, &d);
        if(j <= NUI_SKELETON_POSITION_HAND_RIGHT && j > NUI_SKELETON_POSITION_SPINE)
          emit this->signalDrawJoint(j, (int)x, (int)y, skeletonFrame.SkeletonData[i].eSkeletonPositionTrackingState[j]==NUI_SKELETON_POSITION_TRACKED);
      }
      if(sendframe)
      {
        sendframe = false;
        QString bufferframe = "";

        foreach(int key, bonemapping.keys())
        {
          double px = (skeletonFrame.SkeletonData[i].SkeletonPositions[getJointEnum(bonemapping.value(key))].x)*1000;
          double py = (skeletonFrame.SkeletonData[i].SkeletonPositions[getJointEnum(bonemapping.value(key))].y)*1000;
          double pz = (skeletonFrame.SkeletonData[i].SkeletonPositions[getJointEnum(bonemapping.value(key))].z)*1000;
          if(key == bonemapping.key("NUI_SKELETON_POSITION_FOOT_LEFT") || key == bonemapping.key("NUI_SKELETON_POSITION_FOOT_RIGHT"))
          {
            px = (skeletonFrame.SkeletonData[i].SkeletonPositions[getJointEnum("NUI_SKELETON_POSITION_HIP_CENTER")].x)*1000;
            py = (skeletonFrame.SkeletonData[i].SkeletonPositions[getJointEnum("NUI_SKELETON_POSITION_HIP_CENTER")].y)*1000;
            pz = (skeletonFrame.SkeletonData[i].SkeletonPositions[getJointEnum("NUI_SKELETON_POSITION_HIP_CENTER")].z)*1000;
            py -= 1000;
          }
          bufferframe += " [" + QString::number(key) + " 1]"; // id quality
          bufferframe += " [" + QString::number(-pz + x_loc*1000,'f',3) + " " +
              QString::number(-px + y_loc*1000,'f',3) + " " +
              QString::number(py + z_loc*1000,'f',3) + "]";
          if(key == bonemapping.key("NUI_SKELETON_POSITION_HIP_CENTER"))
            bufferframe += calculateRotation(z_rot,y_rot,x_rot);
          else
            bufferframe +=  calculateRotation(0,0,0);

        }
        bufferframe = "6d " + QString::number(bonemapping.count()) + bufferframe;
        emit this->signalSendARTData(bufferframe);
      }
    }
  }
}
//---------------------------------------------------------------------------------------------------------------------
// Startup slot for kinect thread
//---------------------------------------------------------------------------------------------------------------------
void ARTKinectV1::slotStartKinect()
{
  kinectMutex.lock();

  if(m_pNuiSensor != NULL)
  {
    emit this->signalKinectError("No Kinect");
    return;
  }
  kinectMutex.unlock();

  startKinect();

  while(runkinect)
  {
    update();
    QApplication::processEvents();
  }

  emit this->signalKinectStopped();
}

NUI_SKELETON_POSITION_INDEX ARTKinectV1::getJointEnum(QString name)
{
  if(name == "NUI_SKELETON_POSITION_HIP_CENTER")
    return NUI_SKELETON_POSITION_HIP_CENTER;
  if(name == "NUI_SKELETON_POSITION_HAND_RIGHT")
    return NUI_SKELETON_POSITION_HAND_RIGHT;
  if(name == "NUI_SKELETON_POSITION_FOOT_RIGHT")
    return NUI_SKELETON_POSITION_FOOT_RIGHT;
  if(name == "NUI_SKELETON_POSITION_FOOT_LEFT")
    return NUI_SKELETON_POSITION_FOOT_LEFT;
  if(name == "NUI_SKELETON_POSITION_ELBOW_LEFT")
    return NUI_SKELETON_POSITION_ELBOW_LEFT;
  if(name == "NUI_SKELETON_POSITION_ELBOW_RIGHT")
    return NUI_SKELETON_POSITION_ELBOW_RIGHT;
  if(name == "NUI_SKELETON_POSITION_HAND_LEFT")
    return NUI_SKELETON_POSITION_HAND_LEFT;
  if(name == "NUI_SKELETON_POSITION_HEAD")
    return NUI_SKELETON_POSITION_HEAD;
}
#endif
