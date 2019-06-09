// https://forum.qt.io/topic/66808/qt3d-draw-grid-axis-lines/3

#ifndef DRAWPOINTS_H
#define DRAWPOINTS_H

#include <Qt3DCore/QEntity>
#include <QVector3D>
#include <QColor>

void drawPoints( const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity* _rootEntity );

#endif // DRAWPOINTS_H
