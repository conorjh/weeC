// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "highlighter.h"

#include <QMainWindow>
#include <QMap>
#include <QString>
#include <QSize>

QT_BEGIN_NAMESPACE
class QTextEdit;
QT_END_NAMESPACE

class ToolBar;

QT_FORWARD_DECLARE_CLASS(QAction)
QT_FORWARD_DECLARE_CLASS(QActionGroup)
QT_FORWARD_DECLARE_CLASS(QMenu)

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

public slots:
    void about();
    void newFile();
    void openFile(const QString& path = QString());
    void build();
    void buildAndRun();
    void projectSettings();

private:
    void setupEditor();
    void setupFileMenu();
    void setupBuildMenu();
    void setupProjectMenu();
    void setupViewMenu();
    void setupHelpMenu();
    
    void setupDockWidgets();

    QList<ToolBar*> toolBars;
    QDockWidget* projectDockWidget, *outputDockWidget, *objectBrowserDockWidget;
    QMenu* dockWidgetMenu;
    QList<QDockWidget*> extraDockWidgets;

    QTextEdit *editor;
    Highlighter *highlighter;
};
//! [0]

#endif // MAINWINDOW_H
