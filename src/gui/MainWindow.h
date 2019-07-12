#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QGuiApplication>
#include <QSettings>
#include <QStandardPaths>
#include <QEvent>
#include <QWidget>
#include <QObject>
#include <QCloseEvent>

class MainWindow : public QWidget {
    Q_OBJECT

  public:
    MainWindow( QWidget* parent = nullptr, Qt::WindowFlags f = Qt::WindowFlags() )
      : QWidget( parent, f ) {}

  protected:
    void closeEvent( QCloseEvent* event ) override {
      emit closed();

      event->accept();
    }

  signals:
    void closed();
};

#endif // MAINWINDOW_H
