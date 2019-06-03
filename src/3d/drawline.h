// https://forum.qt.io/topic/66808/qt3d-draw-grid-axis-lines/3

#ifndef DRAWLINE_H
#define DRAWLINE_H

#include <Qt3DCore/QEntity>
#include <QVector3D>
#include <QColor>

void drawLine( const QVector3D& start, const QVector3D& end, const QColor& color, Qt3DCore::QEntity* _rootEntity );

#endif // DRAWLINE_H
