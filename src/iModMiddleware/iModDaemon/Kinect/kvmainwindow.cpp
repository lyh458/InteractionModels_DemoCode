#include "kvmainwindow.h"

#include "kvkinect.h"

KVMainWindow::KVMainWindow(QWidget *parent) : QWidget(parent)
{
  for(int i = 0; i < 4; i++)
    view_kinectlist.append(new KVKinect(this));
  view_merged = view_kinectlist.last();

  view_kinectlist[0]->move(0, 0);
  view_kinectlist[1]->move(windowsx + 2, 0);
  view_kinectlist[2]->move(0, windowsy + 2);
  view_kinectlist[3]->move(windowsx + 2, windowsy + 2);

  this->resize(windowsx * 2 + 2, windowsy * 2 + 2);
  this->show();
}

void KVMainWindow::slotUpdateDrawing(int kid, QByteArray s_data, QByteArray s_tstate)
{
  QList<double> l_data = deserializeQList<double>(s_data);
  QList<char> tstate = deserializeQList<char>(s_tstate);
  QList<QVector3D> data;

  for(int i = 0; i < l_data.count(); i+=3)
    data.append(QVector3D(l_data[i + 0], l_data[i + 1], l_data[i + 2]));

  if(data.count() < 8)
    return;

  if(kid == 1) // Merged (i dont think the router will send kinect data
  {
    for(int i = 0; i < data.count(); i++)
      data[i] = data[i] / 1000.0;
    view_merged->updateDrawing(data.mid(0, 8), tstate.mid(0, 8), kid, QMatrix4x4()); // Only for one user pls...
    return;
  }
  int viewid = view_usage.value(kid, -1);
  if(viewid == -1)
  {
    viewid = getNextFreeView();
    if(viewid == -1)
      qDebug() << "Cant show Skeleton for kinect" << kid << ": Out of Views";
    view_usage.insert(kid, viewid);
  }
  if(viewid != -2)
  {
    view_kinectlist[viewid]->updateDrawing(data, tstate, kid, QMatrix4x4());
    view_merged->updateDrawing(data, tstate, kid, kinecttransitions.value(kid, QMatrix4x4()));
  }
}

void KVMainWindow::slotSetTransitions(QMap<int, QMatrix4x4> _kt)
{
  kinecttransitions = _kt;
}

int KVMainWindow::getNextFreeView()
{
  for(int i = 0; i < 3; i++)
    if(!view_usage.values().contains(i))
      return i;
  return -2;
}
