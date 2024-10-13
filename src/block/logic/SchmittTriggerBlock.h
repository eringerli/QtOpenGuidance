// Copyright( C ) Christian Riggenbach
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include <QObject>

#include "block/BlockBase.h"
#include "helpers/SchmittTrigger.h"

class SchmittTriggerBlock : public BlockBase {
  Q_OBJECT

public:
  explicit SchmittTriggerBlock( const int idHint, const bool systemBlock, const QString type ) : BlockBase( idHint, systemBlock, type ) {}

  void emitConfigSignals() override;

public Q_SLOTS:
  void setValue( NUMBER_SIGNATURE_SLOT );
  void setThreshold( NUMBER_SIGNATURE_SLOT );
  void setValueHysteresis( NUMBER_SIGNATURE_SLOT );
  void setValueLowerThreshold( NUMBER_SIGNATURE_SLOT );
  void setValueUpperThreshold( NUMBER_SIGNATURE_SLOT );

Q_SIGNALS:
  void numberChanged( NUMBER_SIGNATURE_SIGNAL );

public:
  SchmittTrigger< double > schmittTrigger;
};

class SchmittTriggerBlockFactory : public BlockFactory {
  Q_OBJECT

public:
  SchmittTriggerBlockFactory( QThread* thread ) : BlockFactory( thread, false ) { typeColor = TypeColor::Arithmetic; }

  QString getNameOfFactory() const override { return QStringLiteral( "SchmittTriggerBlock" ); }

  QString getPrettyNameOfFactory() const override { return QStringLiteral( "Schmitt Trigger" ); }

  QString getCategoryOfFactory() const override { return QStringLiteral( "Logic" ); }

  virtual std::unique_ptr< BlockBase > createBlock( int idHint = 0 ) override;
};
