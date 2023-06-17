// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <chrono>
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
#include <QThread>

Q_DECLARE_METATYPE(QDockWidget::DockWidgetFeatures)
QT_FORWARD_DECLARE_CLASS(QAction)

//! [0]
MainWindow::MainWindow(QWidget* parent)
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
    if (InterpreterRunning)
        return;

    using namespace std;
    using namespace weec;
    using namespace weec::lex;
    using namespace weec::parse;

    Parsed = wcParseOutput();
    buildDockWidget->show();
    buildDockWidget->raise();
    buildTextEdit->clear();
    outputTextEdit->clear();
    Filename == ""
        ? printToBuild("Starting build...")
        : printToBuild("Starting build... (" + Filename + ")");

    //get text editor contents
    string ScriptBuffer = editor->toPlainText().toStdString();

    //parse
    printToBuild("Parsing...");

    auto parseStart = chrono::system_clock::now();
    wcTokenizer Tokenizer(ScriptBuffer);
    Parsed = wcParser(Tokenizer).Parse();
    auto timeTaken = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - parseStart).count();

    if (Parsed.Error.Code != wcParserErrorCode::None)
    {
        printToBuild("Error code: " + string(to_string(int(Parsed.Error.Code))) + "  " + to_string(Parsed.Error.Code));
        printToBuild(Parsed.Error.Token.to_string() + " (" + to_string(Parsed.Error.Token.StringToken.Line) + "," + to_string(Parsed.Error.Token.StringToken.Column) + ")");
        return;
    }
    printTree(Parsed.AST);

    //write to file
    printToBuild("");
    printToBuild("Build complete");
    printToBuild("Time taken: " + to_string(timeTaken) + string("ms") + "\n");
}


void MainWindow::buildAndRun()
{
    if (InterpreterRunning)
        return;

    build();

    if (Parsed.IsErrored())
        return;

    using namespace std;
    using namespace weec;
    using namespace weec::interpreter;

    outputDockWidget->show();
    outputDockWidget->raise();
    outputTextEdit->clear();

    QThread* thread = new QThread();
    InterpreterWorker* worker = new InterpreterWorker(Parsed, this);
    worker->moveToThread(thread);

    //connect(worker, &InterpreterWorker::error, this, &InterpreterWorker::errorString);    
    connect(worker, &InterpreterWorker::printed, this, &MainWindow::printToOutput);
    connect(thread, &QThread::started, worker, &InterpreterWorker::process);
    connect(worker, &InterpreterWorker::finished, thread, &QThread::quit);
    connect(worker, &InterpreterWorker::finished, worker, &InterpreterWorker::deleteLater);
    connect(worker, &InterpreterWorker::finished, this, [this]() { InterpreterRunning = false; });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);

    thread->start();
    InterpreterRunning = true;

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

    QFile file("../scripts/fibonacci.wc");
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

    buildMenu->addAction(tr("&Run"), QKeySequence(Qt::Key_F5), this, &MainWindow::buildAndRun);
    buildMenu->addAction(tr("&Build"), QKeySequence(Qt::Key_F7), this, &MainWindow::build);
}


void MainWindow::setupViewMenu()
{
    viewMenu = new QMenu(tr("&View"), this);
    menuBar()->addMenu(viewMenu);
    
    auto action = viewMenu->addAction(tr("&Project"), this, &MainWindow::toggleProjectDockWidget);
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
    connect(action, &QAction::toggled, this, &MainWindow::toggleProjectDockWidget);

    action = viewMenu->addAction(tr("&Output"), this, &MainWindow::toggleOutputDockWidget);
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
    connect(action, &QAction::toggled, this, &MainWindow::toggleOutputDockWidget);

    action = viewMenu->addAction(tr("Ob&ject Browser"), this, &MainWindow::toggleObjectBrowserDockWidget);
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
    connect(action, &QAction::toggled, this, &MainWindow::toggleObjectBrowserDockWidget);;

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
    outputDockWidget->setWidget(outputTextEdit = new QTextEdit());
    addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);

    buildDockWidget = new QDockWidget();
    buildDockWidget->setObjectName("Build");
    buildDockWidget->setWindowTitle("Build");
    buildDockWidget->setWidget(buildTextEdit = new QTextEdit());
    addDockWidget(Qt::BottomDockWidgetArea, buildDockWidget);

    tabifyDockWidget(outputDockWidget, buildDockWidget);

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

void MainWindow::toggleProjectDockWidget()
{
    auto toggleAction = projectDockWidget->toggleViewAction();
    projectDockWidget->addAction(toggleAction);
}

void MainWindow::toggleOutputDockWidget()
{
    auto toggleAction = outputDockWidget->toggleViewAction();
    outputDockWidget->addAction(toggleAction);
}

void MainWindow::toggleObjectBrowserDockWidget()
{
    auto toggleAction = objectBrowserDockWidget->toggleViewAction();

    objectBrowserDockWidget->addAction(toggleAction);
}

void MainWindow::printToOutput(std::string Input)
{
    QTextCursor curs(outputTextEdit->textCursor());
    curs.movePosition(outputTextEdit->textCursor().End);
    curs.insertText(std::string(Input + "\n").c_str());
    outputTextEdit->setTextCursor(curs);
}

void MainWindow::printToBuild(std::string Input)
{
    QTextCursor curs(buildTextEdit->textCursor());
    curs.movePosition(buildTextEdit->textCursor().End);
    curs.insertText(std::string(Input + "\n").c_str());
    buildTextEdit->setTextCursor(curs);
}

InterpreterWorker::~InterpreterWorker()
{
    delete Interpreter;
}

void InterpreterWorker::process()
{
    using namespace std;

    emit printed("Executing...");
    auto parseStart = chrono::system_clock::now();

    auto Result = Interpreter->Exec();

    auto timeTaken = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - parseStart).count();
    emit printed("Execution Finished: " + to_string(timeTaken) + "ms");
    emit finished();
}

inline QInterpreter::QInterpreter(weec::parse::wcParseOutput& _Input, InterpreterWorker* _Worker)
    : wcInterpreter(_Input)
{
    Input = _Input;
    PC = Input.AST.begin();
    Halt = false;
    Worker = _Worker;
}

inline void QInterpreter::PrintFunc(std::string In)
{
    emit Worker->printed(In);
}
