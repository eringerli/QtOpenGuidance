// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "qobjectdefs.h"
#include <memory>
#include <utility>
#ifndef Q_MOC_RUN
  #include <ranges>
#endif

#include <QLatin1StringView>
#include <QMetaMethod>
#include <QMetaObject>
#include <QObject>
#include <QString>
#include <QStringLiteral>

#include "kinematic/CalculationOptions.h"

#include "helpers/signatures.h"
#include "qmetaobject.h"

#include <QJsonObject>
#include <utility>

#include <QDebug>

class QJsonObject;

typedef uint16_t BlockBaseId;

struct BlockConnectionDefinition {
  explicit BlockConnectionDefinition( const BlockBaseId idFrom,
                                      const QString     portFromName,
                                      const BlockBaseId idTo,
                                      const QString     portToName )
      : idFrom( idFrom ), portFromName( portFromName ), idTo( idTo ), portToName( portToName ) {}

  BlockConnectionDefinition() = delete;

  BlockConnectionDefinition( const BlockConnectionDefinition& other )
      : idFrom( other.idFrom ), portFromName( other.portFromName ), idTo( other.idTo ), portToName( other.portToName ) {}

  BlockConnectionDefinition( BlockConnectionDefinition&& other )
      : idFrom( other.idFrom )
      , portFromName( std::move( other.portFromName ) )
      , idTo( other.idTo )
      , portToName( std::move( other.portToName ) ) {}

  BlockConnectionDefinition& operator=( const BlockConnectionDefinition& other ) {
    if( idFrom == 0 || idTo == 0 || other.idFrom == 0 || other.idTo == 0 ) {
      qDebug() << "operator=( const BlockConnectionDefinition& other )";
      qDebug() << "idFrom == 0";
    }

    idFrom       = other.idFrom;
    idTo         = other.idTo;
    portFromName = other.portFromName;
    portToName   = other.portToName;

    return *this;
  }

  BlockConnectionDefinition& operator=( BlockConnectionDefinition&& other ) {
    if( idFrom == 0 || idTo == 0 || other.idFrom == 0 || other.idTo == 0 ) {
      qDebug() << "operator=( BlockConnectionDefinition& other )";
      qDebug() << "idFrom == 0";
    }

    idFrom       = other.idFrom;
    portFromName = std::move( other.portFromName );
    idTo         = other.idTo;
    portToName   = std::move( other.portToName );

    return *this;
  }

  bool operator==( BlockConnectionDefinition& other ) {
    if( idFrom == 0 || idTo == 0 || other.idFrom == 0 || other.idTo == 0 ) {
      qDebug() << "operator==( BlockConnectionDefinition& other )";
      qDebug() << "idFrom == 0";
      return false;
    }

    return idFrom == other.idFrom && idTo == other.idTo && portFromName == other.portFromName && portToName == other.portToName;
  }

  bool operator<( BlockConnectionDefinition& other ) {
    if( idFrom == 0 || idTo == 0 || other.idFrom == 0 || other.idTo == 0 ) {
      qDebug() << "operator<( BlockConnectionDefinition& other )";
      qDebug() << "idFrom == 0";
      return false;
    }

    return std::tie( idFrom, idTo, portFromName, portToName ) < std::tie( other.idFrom, other.idTo, other.portFromName, other.portToName );
  }

  const bool operator<( const BlockConnectionDefinition& other ) const {
    if( idFrom == 0 || idTo == 0 || other.idFrom == 0 || other.idTo == 0 ) {
      qDebug() << "operator<( const BlockConnectionDefinition& other ) const";
      qDebug() << "idFrom == 0";
      return false;
    }

    return std::tie( idFrom, idTo, portFromName, portToName ) < std::tie( other.idFrom, other.idTo, other.portFromName, other.portToName );
  }

  BlockBaseId idFrom       = 0;
  QString     portFromName = QString( "" );
  BlockBaseId idTo         = 0;
  QString     portToName   = QString( "" );
};

class BlockPort {
public:
  enum Flag { None = 0, Visible = 0x01, Output = 0x02, SquareBullet = 0x04 };
  Q_DECLARE_FLAGS( Flags, Flag )

  BlockPort( BlockBaseId      idOfBlock,
             uint8_t          position,
             QString          name,
             QObject*         object,
             QMetaMethod      method,
             BlockPort::Flags flags = BlockPort::Flag::Visible )
      : idOfBlock( idOfBlock ), name( name ), flags( flags ), position( position ), object( object ), method( method ) {}

  BlockPort( BlockBaseId idOfBlock, uint8_t position, QString name, BlockPort::Flags flags = BlockPort::Flag::Visible )
      : idOfBlock( idOfBlock ), name( name ), flags( flags ), position( position ), object( nullptr ), method( QMetaMethod() ) {}

  const bool isVisible() const { return flags.testFlag( BlockPort::Flag::Visible ); }
  const bool isOutput() const { return flags.testFlag( BlockPort::Flag::Output ); }
  const bool isSquareBullet() const { return flags.testFlag( BlockPort::Flag::SquareBullet ); }

public:
  QString     name      = QString();
  BlockBaseId idOfBlock = 0;
  Flags       flags     = Flag::Visible;
  uint8_t     position  = 0;
  QObject*    object    = nullptr;
  QMetaMethod method    = QMetaMethod();
};

Q_DECLARE_OPERATORS_FOR_FLAGS( BlockPort::Flags )

class BlockConnection {
public:
  BlockConnection( BlockPort* from, BlockPort* to, QMetaObject::Connection&& connection )
      : portFrom( from ), portTo( to ), connection( std::move( connection ) ) {}

  bool operator==( const BlockConnectionDefinition& other ) const {
    return portFrom->idOfBlock == other.idFrom && portFrom->name == other.portFromName && portTo->idOfBlock == other.idTo &&
           portTo->name == other.portToName;
  }

  const BlockConnectionDefinition getDefinition() const;

  inline void disconnect() { QObject::disconnect( connection ); }

  void disable() { disconnect(); }

  void enable() {
    if( !connection ) {
      connection = QObject::connect( portFrom->object,
                                     portFrom->method,
                                     portTo->object,
                                     portTo->method,
                                     Qt::ConnectionType( Qt::AutoConnection | Qt::UniqueConnection ) );
    }
  }

  void toJSON( QJsonObject& valuesObject ) const;

  BlockPort* portFrom = nullptr;
  BlockPort* portTo   = nullptr;

  QMetaObject::Connection connection;
};

class BlockBase : public QObject {
  Q_OBJECT

public:
  using PortsVectorType = std::vector< BlockPort >;

  enum class IdRange : BlockBaseId { NoId = 0, SystemIdStart = 1, UserIdStart = 1000 };
  enum class TypeColor { Model, Dock, InputDock, Parser, Value, InputOutput, Converter, Arithmetic, System };

  explicit BlockBase( const BlockBaseId idHint, const bool systemBlock, const QString type, const TypeColor typeColor )
      : _id( idHint ), systemBlock( systemBlock ), type( type ), _name( type ), typeColor( typeColor ) {
    if( typeColor != BlockBase::TypeColor::Value ) {
      addInputPort(
        "Enable", this, QLatin1StringView( SLOT( enable( ACTION_SIGNATURE ) ) ), BlockPort::Flag::Visible | BlockPort::Flag::SquareBullet );
    }
  }

  virtual ~BlockBase() {
    for( const auto& object : _additionalObjects ) {
      object->deleteLater();
    }
  }

  virtual void emitConfigSignals() { Q_EMIT nameChanged( _name ); }

  void toJSONBase( QJsonObject& valuesObject ) const;
  void fromJSONBase( const QJsonObject& valuesObject );

  virtual void toJSON( QJsonObject& valuesObject ) const {}
  virtual void fromJSON( const QJsonObject& valuesObject ) {}

  void addAdditionalObject( QObject* object );

  BlockPort& addInputPort( const QString&    name,
                           QObject*          obj       = nullptr,
                           QLatin1StringView signature = QLatin1StringView(),
                           BlockPort::Flags  flags     = BlockPort::Flag::Visible );
  BlockPort& addOutputPort( const QString&    name,
                            QObject*          obj       = nullptr,
                            QLatin1StringView signature = QLatin1StringView(),
                            BlockPort::Flags  flags     = BlockPort::Flag::Visible );
  BlockPort& addPort( const QString&    name,
                      QObject*          obj       = nullptr,
                      QLatin1StringView signature = QLatin1StringView(),
                      BlockPort::Flags  flags     = BlockPort::Flag::Visible );

  auto getOutputPorts( const QString& name ) const {
    return _ports | std::ranges::views::filter( [&name]( const auto& port ) { return port.isOutput() && ( port.name == name ); } );
  }

  auto getInputPorts( const QString& name ) const {
    return _ports | std::ranges::views::filter( [&name]( const auto& port ) { return !port.isOutput() && ( port.name == name ); } );
  }

  size_t connectPortToPort( const QString& portFromName, const BlockBase* blockTo, const QString& portToName );
  void   disconnectPortToPort( const QString& portFromName, const BlockBase* blockTo, const QString& portToName );

public slots:
  virtual void enable( const bool enable );

signals:
  void nameChanged( const QString& );

public:
  const bool      systemBlock = false;
  const QString   type;
  const TypeColor typeColor;

  const std::vector< QObject* >&        additionalObjects() const { return _additionalObjects; }
  const std::vector< BlockPort >&       ports() const { return _ports; }
  const std::vector< BlockConnection >& connections() const { return _connections; }
  std::vector< BlockConnection >&       connections() { return _connections; }

  const BlockConnection* getConnection( const BlockConnectionDefinition connectionDefinition );

  double positionX = 0;
  double positionY = 0;

  const BlockBaseId id() const { return _id; }
  void              setId( BlockBaseId newId );

  const QString& name() const { return _name; }
  virtual void   setName( const QString& name );

private:
  BlockBaseId _id = static_cast< std::underlying_type_t< IdRange > >( IdRange::NoId );
  QString     _name;

  uint8_t _portPosition = 0;

  std::vector< QObject* >        _additionalObjects;
  std::vector< BlockPort >       _ports;
  std::vector< BlockConnection > _connections;
};

class BlockFactory : public QObject {
  Q_OBJECT

public:
  explicit BlockFactory( QThread* thread, bool systemBlock ) : thread( thread ), systemBlock( systemBlock ) {}
  virtual ~BlockFactory() {}

  virtual QString getNameOfFactory() const = 0;

  virtual QString getPrettyNameOfFactory() const { return getNameOfFactory(); }

  virtual QString getCategoryOfFactory() const = 0;

  virtual std::unique_ptr< BlockBase > createBlock( BlockBaseId idHint = 0 ) = 0;

  template< class BlockClass, class... Args >
  [[nodiscard]] std::unique_ptr< BlockClass > createBaseBlock( BlockBaseId idHint, Args... args ) {
    return std::make_unique< BlockClass >( args..., idHint, systemBlock, getNameOfFactory(), typeColor );
  }

  void moveToFactoryThread( QObject* obj ) { obj->moveToThread( thread ); }

public:
  QThread*             thread      = nullptr;
  bool                 systemBlock = false;
  BlockBase::TypeColor typeColor   = BlockBase::TypeColor::Arithmetic;
};
