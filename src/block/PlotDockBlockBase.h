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

#pragma once

#include <QObject>
#include <QSizePolicy>
#include <QMenu>

#include "../gui/MyMainWindow.h"
#include "../gui/PlotDock.h"

#include "BlockBase.h"

#include <kddockwidgets/KDDockWidgets.h>
#include <kddockwidgets/DockWidget.h>

#include "qcustomplot.h"

class PlotDockBlockBase : public BlockBase {
    Q_OBJECT

  public:
    explicit PlotDockBlockBase( const QString& uniqueName,
                                MyMainWindow* mainWindow )
      : BlockBase() {
      dock = new KDDockWidgets::DockWidget( uniqueName );
      widget = new PlotDock( mainWindow );
    }

    virtual ~PlotDockBlockBase() {
      widget->deleteLater();

      if( PlotDockBlockBase::firstPlotDock == dock ) {
        PlotDockBlockBase::firstPlotDock = nullptr;
      }

      dock->deleteLater();
    }

    void toJSON( QJsonObject& json ) override {
      QJsonObject valuesObject;

      valuesObject[QStringLiteral( "XAxisVisible" )] = getXAxisVisible();
      valuesObject[QStringLiteral( "YAxisVisible" )] = getYAxisVisible();
      valuesObject[QStringLiteral( "YAxisDescription" )] = getYAxisDescription();
      valuesObject[QStringLiteral( "AutoscrollEnabled" )] = getAutoscrollEnabled();
      valuesObject[QStringLiteral( "Window" )] = getWindow();

      json[QStringLiteral( "values" )] = valuesObject;
    }

    void fromJSON( QJsonObject& json ) override {
      if( json[QStringLiteral( "values" )].isObject() ) {
        QJsonObject valuesObject = json[QStringLiteral( "values" )].toObject();

        setXAxisVisible( valuesObject[QStringLiteral( "XAxisVisible" )].toBool( false ) );
        setYAxisVisible( valuesObject[QStringLiteral( "YAxisVisible" )].toBool( true ) );
        setYAxisDescription( valuesObject[QStringLiteral( "YAxisDescription" )].toString( QString() ) );
        setAutoscrollEnabled( valuesObject[QStringLiteral( "AutoscrollEnabled" )].toBool( true ) );
        setWindow( valuesObject[QStringLiteral( "Window" )].toDouble( 20 ) );
      }
    }

  public:
    bool getXAxisVisible() {
      return widget->getQCustomPlotWidget()->xAxis->visible();
    }
    bool getYAxisVisible() {
      return widget->getQCustomPlotWidget()->yAxis->visible();
    }
    const QString getYAxisDescription() {
      return widget->getQCustomPlotWidget()->yAxis->label();
    }
    bool getAutoscrollEnabled() {
      return autoScrollEnabled;
    }
    double getWindow() {
      return window;
    }

    void setXAxisVisible( bool visible ) {
      widget->getQCustomPlotWidget()->xAxis->setVisible( visible );
    }
    void setYAxisVisible( bool visible ) {
      widget->getQCustomPlotWidget()->yAxis->setVisible( visible );
    }
    void setYAxisDescription( const QString& description ) {
      widget->getQCustomPlotWidget()->yAxis->setLabel( description );
    }
    void setAutoscrollEnabled( bool enabled ) {
      autoScrollEnabled = enabled;

      if( !enabled ) {
        widget->getQCustomPlotWidget()->setInteractions( QCP::Interaction::iRangeDrag | QCP::Interaction::iRangeZoom );
      } else {
        widget->getQCustomPlotWidget()->setInteractions( QCP::Interaction() );
      }

      setNameHelper();
    }
    void setWindow( double window ) {
      this->window = window;
    }

  public slots:
    void setName( const QString& name ) override {
      this->name = name;
      setNameHelper();
    }

    void qCustomPlotWidgetMouseDoubleClick( QMouseEvent* ) {
      setAutoscrollEnabled( !autoScrollEnabled );
    }

  private:
    void setNameHelper() {
      dock->setTitle( name + ( autoScrollEnabled ? QString() : QStringLiteral( " (m)" ) ) );
      dock->toggleAction()->setText( QStringLiteral( "Plot: " ) + name );
    }

  public:
    KDDockWidgets::DockWidget* dock = nullptr;
    PlotDock* widget = nullptr;

    QString name;

    bool autoScrollEnabled = true;
    double window = 20;
    bool interactable = false;

    static KDDockWidgets::DockWidget* firstPlotDock;
};
