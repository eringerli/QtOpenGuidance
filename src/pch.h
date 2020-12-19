// Copyright( C ) 2020 Christian Riggenbach
//
// This program is free software:
// you can redistribute it and / or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// ( at your option ) any later version.
//
// This program is distributed in the hope that it will be useful,
//      but WITHOUT ANY WARRANTY;
// without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see < https : //www.gnu.org/licenses/>.

// command to make the headers unique and sorted: cat pch.h | sort -u

#ifdef __cplusplus

#include <algorithm>
#include "helpers/cgalHelper.h"
#include "helpers/eigenHelper.h"
#include "kinematic/cgal.h"
#include "kinematic/cgalKernel.h"
#include <climits>
#include <cmath>
#include <iostream>
#include <GeographicLib/Ellipsoid.hpp>
#include <GeographicLib/LocalCartesian.hpp>
#include <GeographicLib/TransverseMercator.hpp>
#include <GeographicLib/UTMUPS.hpp>
#include <kddockwidgets/DockWidget.h>
#include <kddockwidgets/FrameworkWidgetFactory.h>
#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/MainWindow.h>
#include <kddockwidgets/private/TitleBar_p.h>
#include <memory>
#include <QBasicTimer>
#include <QBoxLayout>
#include <QButtonGroup>
#include <QByteArray>
#include <QCloseEvent>
#include <QColor>
#include <QComboBox>
#include <QDebug>
#include <QDialog>
#include <QDockWidget>
#include <QElapsedTimer>
#include <QEvent>
#include <QFlags>
#include <QFontComboBox>
#include <QGeometryRenderer>
#include <QGraphicsScene>
#include <QGridLayout>
#include <QGroupBox>
#include <QGuiApplication>
#include <QJsonDocument>
#include <QJsonObject>
#include <QLabel>
#include <QLatin1String>
#include <QLineF>
#include <QList>
#include <QMainWindow>
#include <QMenu>
#include <QMutex>
#include <QMutexLocker>
#include <QNode>
#include <QObject>
#include <QPaintedTextureImage>
#include <QPainter>
#include <QPainterPath>
#include <QPalette>
#include <QPointF>
#include <QPolygonF>
#include <QPushButton>
#include <Eigen/Geometry>
#include <QSettings>
#include <QSharedPointer>
#include <QSize>
#include <QSizePolicy>
#include <QSortFilterProxyModel>
#include <QStandardPaths>
#include <QString>
#include <QStringLiteral>
#include <QStyledItemDelegate>
#include <Qt3DCore/QComponent>
#include <Qt3DCore/QEntity>
#include <Qt3DCore/QTransform>
#include <Qt3DExtras/QCuboidMesh>
#include <Qt3DExtras/QCylinderMesh>
#include <Qt3DExtras/QDiffuseSpecularMaterial>
#include <Qt3DExtras/QExtrudedTextMesh>
#include <Qt3DExtras/qfirstpersoncameracontroller.h>
#include <Qt3DExtras/QOrbitCameraController>
#include <Qt3DExtras/QPhongMaterial>
#include <Qt3DExtras/QSphereMesh>
#include <Qt3DLogic/QFrameAction>
#include <Qt3DRender/QAttribute>
#include <Qt3DRender/QBuffer>
#include <Qt3DRender/QCamera>
#include <Qt3DRender/QGeometry>
#include <Qt3DRender/QGeometryRenderer>
#include <Qt3DRender/QPointLight>
#include <Qt3DRender/QTexture>
#include <Qt3DRender/QTextureWrapMode>
#include <QtCore/QAbstractTableModel>
#include <QtDebug>
#include <QTextStream>
#include <QtGlobal>
#include <QThread>
#include <QTime>
#include <QtMath>
#include <QToolButton>
#include <QtWidgets>
#include <QVector>
#include <QVector3D>
#include <QWidget>
#include <string>
#include <utility>
#include <vector>

#endif
