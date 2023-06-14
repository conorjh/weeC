// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <QtWidgets>
#include "app.h"
#include "mainwindow.h"

#include <QActionGroup>
#include <QLayout>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTextEdit>
#include <QFile>
#include <QDataStream>
#include <QFileDialog>
#include <QDialogButtonBox>
#include <QMessageBox>
#include <QApplication>
#include <QPainter>
#include <QMouseEvent>
#include <QLineEdit>
#include <QComboBox>
#include <QLabel>
#include <QPushButton>
#include <QTextEdit>
#include <QDebug>

Q_DECLARE_METATYPE(QDockWidget::DockWidgetFeatures)
QT_FORWARD_DECLARE_CLASS(QAction)

//! [0]
MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setupFileMenu();
    setupBuildMenu();
    setupProjectMenu();
    setupViewMenu();
    setupHelpMenu();
    setupDockWidgets();

    setupEditor();

    setCentralWidget(editor);
    setWindowTitle(tr(App::AppTitle));
}
//! [0]

void MainWindow::about()
{
    QMessageBox::about(this, tr("About"), tr("<p>The <b>weeC IDE</b>" ));
}

void MainWindow::newFile()
{
    editor->clear();
}

void MainWindow::openFile(const QString &path)
{
    QString fileName = path;

    if (fileName.isNull())
        fileName = QFileDialog::getOpenFileName(this, tr("Open File"), "","weeC Files (*.wc *.wh);;weeC Projects (*.wcp)" );

    if (!fileName.isEmpty()) {
        QFile file(fileName);
        if (file.open(QFile::ReadOnly | QFile::Text))
            editor->setPlainText(file.readAll());
    }
}

void MainWindow::build()
{
}

void MainWindow::buildAndRun()
{
}

void MainWindow::projectSettings()
{
}


//! [1]
void MainWindow::setupEditor()
{
    QFont font;
    font.setFamily("Courier");
    font.setFixedPitch(true);
    font.setPointSize(10);

    editor = new QTextEdit;
    editor->setFont(font);

    highlighter = new Highlighter(editor->document());

    QFile file("mainwindow.h");
    if (file.open(QFile::ReadOnly | QFile::Text))
        editor->setPlainText(file.readAll());
}
//! [1]

void MainWindow::setupFileMenu()
{
    QMenu* fileMenu = new QMenu(tr("&File"), this);
    menuBar()->addMenu(fileMenu);

    fileMenu->addAction(tr("&New"), QKeySequence::New,
        this, &MainWindow::newFile);
    fileMenu->addAction(tr("&Open..."), QKeySequence::Open,
        this, [this]() { openFile(); });
    fileMenu->addAction(tr("E&xit"), QKeySequence::Quit,
        qApp, &QApplication::quit);
}

void MainWindow::setupBuildMenu()
{
    QMenu* buildMenu = new QMenu(tr("&Build"), this);
    menuBar()->addMenu(buildMenu);

    buildMenu->addAction(tr("&Build"), this, &MainWindow::build);
    buildMenu->addAction(tr("&Build and Run"), this, &MainWindow::buildAndRun);
}


void MainWindow::setupViewMenu()
{
    QMenu* buildMenu = new QMenu(tr("&View"), this);
    menuBar()->addMenu(buildMenu);
    
    auto action = buildMenu->addAction(tr("&Project"), this, &MainWindow::build);
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
    //connect(action, &QAction::toggled, this, &MainWindow::setDockOptions);

    action = buildMenu->addAction(tr("&Output"), this, &MainWindow::build);
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
   // connect(action, &QAction::toggled, this, &MainWindow::setDockOptions);

    action = buildMenu->addAction(tr("Ob&ject Browser"), this, &MainWindow::build);
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
   // connect(action, &QAction::toggled, this, &MainWindow::);

}

void MainWindow::setupProjectMenu()
{
    QMenu* buildMenu = new QMenu(tr("&Project"), this);
    menuBar()->addMenu(buildMenu);

    buildMenu->addAction(tr("&Settings"), this, &MainWindow::projectSettings);
}

void MainWindow::setupHelpMenu()
{
    QMenu *helpMenu = new QMenu(tr("&Help"), this);
    menuBar()->addMenu(helpMenu);

    helpMenu->addAction(tr("&About"), this, &MainWindow::about);
    helpMenu->addAction(tr("About &Qt"), qApp, &QApplication::aboutQt);
}

void MainWindow::setupDockWidgets()
{
    qRegisterMetaType<QDockWidget::DockWidgetFeatures>();

    outputDockWidget = new QDockWidget();
    outputDockWidget->setObjectName("Output");
    outputDockWidget->setWindowTitle("Output");
    outputDockWidget->setWidget(new QTextEdit);
    addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);

    projectDockWidget = new QDockWidget();
    projectDockWidget->setObjectName("Project");
    projectDockWidget->setWindowTitle("Project");
    projectDockWidget->setWidget(new QTextEdit);
    addDockWidget(Qt::RightDockWidgetArea, projectDockWidget);

    objectBrowserDockWidget = new QDockWidget();
    objectBrowserDockWidget->setObjectName("Object Browser");
    objectBrowserDockWidget->setWindowTitle("Object Browser");
    objectBrowserDockWidget->setWidget(new QTextEdit);
    addDockWidget(Qt::RightDockWidgetArea, objectBrowserDockWidget);

    tabifyDockWidget(objectBrowserDockWidget, projectDockWidget);
}
