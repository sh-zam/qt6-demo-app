#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QLayout>
#include "TheOpenGlWidget.h"
#include "dialog.h"
#include <QDockWidget>
#include <QtQuickWidgets/QQuickWidget>
#include <QQmlEngine>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
    setCentralWidget(new TheOpenGlWidget);

    QDockWidget *dw = new QDockWidget;
    dw->setWidget(new Dialog);
    addDockWidget(Qt::LeftDockWidgetArea, dw);

    QDockWidget *dw2 = new QDockWidget;
    QQuickWidget *qw = new QQuickWidget(QUrl("qrc:main.qml"), dw2);
    qw->setResizeMode(QQuickWidget::SizeRootObjectToView);
    dw2->setWidget(qw);
    addDockWidget(Qt::RightDockWidgetArea, dw2);
}

MainWindow::~MainWindow()
{
    delete ui;
}
