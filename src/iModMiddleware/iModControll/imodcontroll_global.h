/**
* @file iModControll/imodcontroll_global.h
* @brief Header file for DLL_EXPORT on different plattforms
*/

#ifndef IMODCONTROLL_GLOBAL_H
#define IMODCONTROLL_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(IMODCONTROLL_LIBRARY)
#  define IMODCONTROLLSHARED_EXPORT Q_DECL_EXPORT
#else
#  define IMODCONTROLLSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // IMODCONTROLL_GLOBAL_H
