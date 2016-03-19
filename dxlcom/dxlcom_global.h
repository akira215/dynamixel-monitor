#ifndef DXLCOM_GLOBAL_H
#define DXLCOM_GLOBAL_H

#include <QtCore/QtGlobal>

#if defined(DXLCOM_LIBRARY)
#  define DXLCOMSHARED_EXPORT Q_DECL_EXPORT
#else
#  define DXLCOMSHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // DXLCOM_GLOBAL_H
