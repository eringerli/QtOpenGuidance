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

#include <QApplication>
#include <QMap>
#include <QMetaMethod>
#include <QMetaObject>
#include <QObject>
// #include <QMetaCallEvent>
#include <QSet>
#include <private/qcoreapplication_p.h>
#include <private/qobject_p.h>
#include <private/qthread_p.h>

#include <algorithm>
#include <vector>

#pragma once

/*! Keeps a list of singal indices for one or more meatobject classes.
 * The indices are signal indices as given by QMetaCallEvent.signalId.
 * On Qt 5, those do *not* match QMetaObject::methodIndex since they
 * exclude non-signal methods. */
class SignalList {
  Q_DISABLE_COPY( SignalList )
  typedef QMap< const QMetaObject*, QSet< int > > T;

  T m_data;

  /*! Returns a signal index that is can be compared to QMetaCallEvent.signalId. */
  static int signalIndex( const QMetaMethod& method ) {
    //    Q_ASSERT( method.methodType() == QMetaMethod::Signal || method.methodType() == QMetaMethod::Slot );

    int index = -1;

    const QMetaObject* mobj = method.enclosingMetaObject();

    for( int i = 0; i <= method.methodIndex(); ++i ) {
      auto type = mobj->method( i ).methodType();
      if( type != QMetaMethod::Signal && type != QMetaMethod::Slot ) {
        continue;
      }
      ++index;
    }
    return index;
  }

public:
  SignalList() {}

  void add( const QMetaMethod& method ) { m_data[method.enclosingMetaObject()].insert( signalIndex( method ) ); }

  void remove( const QMetaMethod& method ) {
    T::iterator it = m_data.find( method.enclosingMetaObject() );
    if( it != m_data.end() ) {
      it->remove( signalIndex( method ) );
      if( it->empty() ) {
        m_data.erase( it );
      }
    }
  }

  bool contains( const QMetaObject* metaObject, int signalId ) {
    T::const_iterator it = m_data.find( metaObject );
    return it != m_data.end() && it.value().contains( signalId );
  }

  bool empty() { return m_data.empty(); }
};

//
// Implementation Using Event Compression With Access to Private Qt Headers

struct EventHelper : private QEvent {
  static void clearPostedFlag( QEvent* ev ) {
    ( &static_cast< EventHelper* >( ev )->t )[1] &= ~0x8001; // Hack to clear QEvent::posted
  }
};

template< class Base >
class CompressorApplication : public Base {
  SignalList m_compressedSignals;

  std::vector< QObject* > objectsToCompress;

public:
  CompressorApplication( int& argc, char** argv ) : Base( argc, argv ) {}
  void addCompressedSignal( const QMetaMethod& method ) { m_compressedSignals.add( method ); }
  void removeCompressedSignal( const QMetaMethod& method ) { m_compressedSignals.remove( method ); }

  void addCompressedObject( QObject* object ) { objectsToCompress.push_back( object ); }

protected:
  virtual bool compressEvent( QEvent* event, QObject* receiver, QPostEventList* postedEvents ) override {
    if( ( event->type() == QEvent::Type::MetaCall ) &&
        ( std::find( objectsToCompress.cbegin(), objectsToCompress.cend(), receiver ) != objectsToCompress.cend() ) ) {
      auto* mce = reinterpret_cast< QMetaCallEvent* >( event );

      for( auto it = postedEvents->begin(); it != postedEvents->end(); ++it ) {
        QPostEvent& cur = *it;
        if( cur.receiver != receiver || cur.event == 0 || cur.event->type() != event->type() ) {
          continue;
        }

        QMetaCallEvent* cur_mce = static_cast< QMetaCallEvent* >( cur.event );
        if( cur_mce->sender() != mce->sender() || cur_mce->signalId() != mce->signalId() || cur_mce->id() != mce->id() ) {
          continue;
        }

        if( true ) {
          /* Keep The Newest Call */
          // We can't merely qSwap the existing posted event with the new one, since QEvent
          // keeps track of whether it has been posted. Deletion of a formerly posted event
          // takes the posted event list mutex and does a useless search of the posted event
          // list upon deletion. We thus clear the QEvent::posted flag before deletion.
          EventHelper::clearPostedFlag( cur.event );
          delete cur.event;
          cur.event = event;
        } else {
          /* Keep the Oldest Call */
          delete event;
        }

        return true;
      }
    } else {
      return Base::compressEvent( event, receiver, postedEvents );
    }

    return false;
  }
};

inline void
addCompressedSignal( const QMetaMethod& method ) {
  ( ( CompressorApplication< QApplication >* )QApplication::instance() )->addCompressedSignal( method );
}

inline void
removeCompressedSignal( const QMetaMethod& method ) {
  ( ( CompressorApplication< QApplication >* )QApplication::instance() )->removeCompressedSignal( method );
}

inline void
addCompressedObject( QObject* object ) {
  ( ( CompressorApplication< QApplication >* )QApplication::instance() )->addCompressedObject( object );
}
