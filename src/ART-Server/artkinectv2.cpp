#ifdef WIN32
#include "artkinectv2.h"

#include <QtCore/QDebug>
#include <QtWidgets/QApplication>

#include <profileapi.h>

ARTKinectV2::ARTKinectV2(QMap<int, QString> bonemapping, bool fullbody) :
  ARTKinectBase(bonemapping, fullbody),
  m_pKinectSensor(NULL),
  m_pCoordinateMapper(NULL),
  m_pBodyFrameReader(NULL),
  m_nStartTime(0),
  m_fFreq(0),
  m_nLastCounter(0),
  m_nFramesSinceUpdate(0)
{
  LARGE_INTEGER qpf = {0};
  if (QueryPerformanceFrequency(&qpf))
  {
      m_fFreq = double(qpf.QuadPart);
  }
}

HRESULT ARTKinectV2::startKinect()
{
  HRESULT hr;

  hr = GetDefaultKinectSensor(&m_pKinectSensor);
  if (FAILED(hr))
  {
    return hr;
  }

  if (m_pKinectSensor)
  {
    // Initialize the Kinect and get coordinate mapper and the body reader
    IBodyFrameSource* pBodyFrameSource = NULL;

    hr = m_pKinectSensor->Open();

    if (SUCCEEDED(hr))
    {
      hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
    }

    if (SUCCEEDED(hr))
    {
      hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
    }

    if (SUCCEEDED(hr))
    {
      hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
    }

    safeRelease(pBodyFrameSource);
  }

  if (!m_pKinectSensor || FAILED(hr))
  {
    emit this->signalKinectError("No ready Kinect found!");
    return E_FAIL;
  }

  emit this->signalKinectRunning();
  runkinect = true;

  return hr;
}

void ARTKinectV2::update()
{
  if (!m_pBodyFrameReader)
  {
    return;
  }

  IBodyFrame* pBodyFrame = NULL;

  HRESULT hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

  if (SUCCEEDED(hr))
  {
    INT64 nTime = 0;

    hr = pBodyFrame->get_RelativeTime(&nTime);

    IBody* ppBodies[BODY_COUNT] = {0};

    if (SUCCEEDED(hr))
    {
      hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
    }

    if (SUCCEEDED(hr))
    {
      processSkeleton(nTime, BODY_COUNT, ppBodies);
    }

    for (int i = 0; i < _countof(ppBodies); ++i)
    {
      safeRelease(ppBodies[i]);
    }
  }

  safeRelease(pBodyFrame);
}

void ARTKinectV2::processSkeleton(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
  HRESULT hr;
  if (m_pCoordinateMapper)
  {
    bool map = true;
    for (int i = 0; i < nBodyCount && map; ++i)
    {
      IBody* pBody = ppBodies[i];
      if (pBody)
      {
        BOOLEAN bTracked = false;
        hr = pBody->get_IsTracked(&bTracked);

        if (SUCCEEDED(hr) && bTracked)
        {
          Joint joints[JointType_Count];
          QPoint jointPoints[JointType_Count];
          HandState leftHandState = HandState_Unknown;
          HandState rightHandState = HandState_Unknown;

          pBody->get_HandLeftState(&leftHandState);
          pBody->get_HandRightState(&rightHandState);

          hr = pBody->GetJoints(_countof(joints), joints);
          if (SUCCEEDED(hr) &&
              joints[JointType_SpineBase].Position.Z < maxdistance &&
              joints[JointType_SpineBase].Position.Z > mindistance )
          {
            map = false;
            for (int j = 0; j < _countof(joints); ++j)
            {
              jointPoints[j] = BodyToScreen(joints[j].Position, 490, 368);
              emit this->signalDrawJoint(j, jointPoints[j].x(), jointPoints[j].y(),
                                         joints[j].TrackingState==TrackingState_NotTracked?false:true,
                                         joints[j].TrackingState==TrackingState_Inferred?true:false);
            }
            if(sendframe)
            {
              sendframe = false;
              QString bufferframe = "";
              foreach(int key, bonemapping.keys())
              {
                bufferframe += " [" + QString::number(key) + " 1]"; // id quality
                double px = (joints[getJointEnum(bonemapping.value(key))].Position.X)*1000;
                double py = (joints[getJointEnum(bonemapping.value(key))].Position.Y)*1000;
                double pz = (joints[getJointEnum(bonemapping.value(key))].Position.Z)*1000;
                if((key == bonemapping.key("JointType_FootLeft")   ||
                    key == bonemapping.key("JointType_FootRight")) &&
                    !fullbodytracking
                   )
                {
                  px = (joints[getJointEnum("JointType_SpineBase")].Position.X)*1000;
                  py = (joints[getJointEnum("JointType_SpineBase")].Position.Y)*1000;
                  pz = (joints[getJointEnum("JointType_SpineBase")].Position.Z)*1000;
                  py -= 1000;
                }
                bufferframe += " [" + QString::number(-pz+x_loc*1000,'f',3) + " " +
                    QString::number(-px+y_loc*1000,'f',3) + " " +
                    QString::number(py+z_loc*1000,'f',3) + "]";
                if(key == bonemapping.key("JointType_SpineBase"))
                  bufferframe += calculateRotation(x_rot,y_rot,z_rot);
                else
                  bufferframe +=  calculateRotation(0,0,0);

              }
              bufferframe = "6d " + QString::number(bonemapping.count()) + bufferframe;
              emit this->signalSendARTData(bufferframe);
            }
          }
        }
      }
    }

    // Device lost, need to recreate the render target
    // We'll dispose it now and retry drawing
    if (D2DERR_RECREATE_TARGET == hr)
    {
      hr = S_OK;
    }
  }

  if (!m_nStartTime)
  {
    m_nStartTime = nTime;
  }

  double fps = 0.0;

  LARGE_INTEGER qpcNow = {0};
  if (m_fFreq)
  {
    if (QueryPerformanceCounter(&qpcNow))
    {
      if (m_nLastCounter)
      {
        m_nFramesSinceUpdate++;
        fps = m_fFreq * m_nFramesSinceUpdate / double(qpcNow.QuadPart - m_nLastCounter);
      }
    }
  }

//  WCHAR szStatusMessage[64];
//  qDebug() << fps; //StringCchPrintf(szStatusMessage, _countof(szStatusMessage), L" FPS = %0.2f    Time = %I64d", fps, (nTime - m_nStartTime));
//  evlt für nen label interessant;
//  if (SetStatusMessage(szStatusMessage, 1000, false))
//  {
//    m_nLastCounter = qpcNow.QuadPart;
//    m_nFramesSinceUpdate = 0;
//  }
}

QPoint ARTKinectV2::BodyToScreen(const CameraSpacePoint& bodyPoint, int width, int height)
{
    // Calculate the body's position on the screen
    DepthSpacePoint depthPoint = {0};
    m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);

    float screenPointX = static_cast<float>(depthPoint.X * width) / cDepthWidth;
    float screenPointY = static_cast<float>(depthPoint.Y * height) / cDepthHeight;

    return QPoint(screenPointX, screenPointY);
}

void ARTKinectV2::slotStartKinect()
{
  kinectMutex.lock();

  if(m_pKinectSensor != NULL)
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

JointType ARTKinectV2::getJointEnum(QString name)
{
  if(name == "JointType_SpineBase")
    return JointType_SpineBase;
  if(name == "JointType_HandRight")
    return JointType_HandRight;
  if(name == "JointType_FootRight")
    return JointType_FootRight;
  if(name == "JointType_FootLeft")
    return JointType_FootLeft;
  if(name == "JointType_ElbowLeft")
    return JointType_ElbowLeft;
  if(name == "JointType_ElbowRight")
    return JointType_ElbowRight;
  if(name == "JointType_HandLeft")
    return JointType_HandLeft;
  if(name == "JointType_Head")
    return JointType_Head;
}
#endif
