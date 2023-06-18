// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#include <chrono>
#include <any>
#include <QtWidgets>
#include "app.h"
#include "mainwindow.h"
#include "treemodel.h"
#include "treeitem.h"
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
QT_FORWARD_DECLARE_CLASS(QTreeView)

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

bool MainWindow::saveFile()
{
    if (Filename == "" || Filename.substr(0,2) == std::string(":/"))
        return saveFileAs();

    QTextDocumentWriter writer(Filename.c_str());
    bool success = writer.write(editor->document());
    if (success) {
        editor->document()->setModified(false);
        statusBar()->showMessage(tr("Wrote \"%1\"").arg(QDir::toNativeSeparators(Filename.c_str())));
    }
    else {
        statusBar()->showMessage(tr("Could not write to file \"%1\"")
            .arg(QDir::toNativeSeparators(Filename.c_str())));
    }
    return success;
}

bool MainWindow::saveFileAs()
{
    QFileDialog fileDialog(this, tr("Save as..."));
    fileDialog.setAcceptMode(QFileDialog::AcceptSave);
    fileDialog.setNameFilter("weeC Source (*.wc);;weeC Header (*.wh);;weeC Project (*.wcp)");
    fileDialog.setDefaultSuffix("wc");

    if (fileDialog.exec() != QDialog::Accepted)
        return false;

    Filename = fileDialog.selectedFiles().constFirst().toStdString();
    return saveFile();
}

void MainWindow::buildAndRun()
{
    RunFlag = true;
    build();
}

void MainWindow::build()
{
    if (InterpreterRunning || ParserRunning)
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

    QThread* thread = new QThread();
    ParserWorker* worker = new ParserWorker(ScriptBuffer, Parsed);
    worker->moveToThread(thread);

    //connect(worker, &ParserWorker::error, this, &ParserWorker::errorString);    
    connect(worker, &ParserWorker::printed, this, &MainWindow::printToBuild);
    connect(thread, &QThread::started, worker, &ParserWorker::process);
    connect(worker, &ParserWorker::finished, thread, &QThread::quit);
    connect(worker, &ParserWorker::finished, worker, &ParserWorker::deleteLater);
    connect(worker, &ParserWorker::finished, this, [this]() { ParserRunning = false; });
    connect(thread, &QThread::finished, thread, &QThread::deleteLater);
    if(RunFlag)
        connect(worker, &ParserWorker::finished, this, &MainWindow::run);   //trigger the run once ere finished parsing
    RunFlag = false;
    thread->start();
    ParserRunning = true;
}


void MainWindow::run()
{
    if (InterpreterRunning || ParserRunning || Parsed.IsErrored())
        return;

    using namespace std;
    using namespace weec;
    using namespace weec::interpreter;

    outputDockWidget->show();
    outputDockWidget->raise();
    outputTextEdit->clear();

    QThread* thread = new QThread();
    InterpreterWorker* worker = new InterpreterWorker(Parsed);
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

void MainWindow::insertChild()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel* model = view->model();

    if (model->columnCount(index) == 0) {
        if (!model->insertColumn(0, index))
            return;
    }

    if (!model->insertRow(0, index))
        return;

    for (int column = 0; column < model->columnCount(index); ++column) 
    {
        const QModelIndex child = model->index(0, column, index);
        model->setData(child, QVariant(tr("[No data]")), Qt::EditRole);
        if (!model->headerData(column, Qt::Horizontal).isValid())
            model->setHeaderData(column, Qt::Horizontal, QVariant(tr("[No header]")), Qt::EditRole);
    }

    view->selectionModel()->setCurrentIndex(model->index(0, 0, index),
        QItemSelectionModel::ClearAndSelect);
    updateActions();
}

bool MainWindow::insertColumn()
{
    QAbstractItemModel* model = view->model();
    int column = view->selectionModel()->currentIndex().column();

    // Insert a column in the parent item.
    bool changed = model->insertColumn(column + 1);
    if (changed)
        model->setHeaderData(column + 1, Qt::Horizontal, QVariant("[No header]"), Qt::EditRole);

    updateActions();

    return changed;
}

void MainWindow::insertRow()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel* model = view->model();

    if (!model->insertRow(index.row() + 1, index.parent()))
        return;

    updateActions();

    for (int column = 0; column < model->columnCount(index.parent()); ++column)
    {
        const QModelIndex child = model->index(index.row() + 1, column, index.parent());
        model->setData(child, QVariant(tr("Data")), Qt::EditRole);
    }
}

bool MainWindow::removeColumn()
{
    QAbstractItemModel* model = view->model();
    const int column = view->selectionModel()->currentIndex().column();

    // Insert columns in each child of the parent item.
    const bool changed = model->removeColumn(column);
    if (changed)
        updateActions();

    return changed;
}

void MainWindow::removeRow()
{
    const QModelIndex index = view->selectionModel()->currentIndex();
    QAbstractItemModel* model = view->model();
    if (model->removeRow(index.row(), index.parent()))
        updateActions();
}

void MainWindow::populateTreeView()
{

}

void MainWindow::updateActions()
{
    const bool hasSelection = !view->selectionModel()->selection().isEmpty();
    removeRowAction->setEnabled(hasSelection);
    removeColumnAction->setEnabled(hasSelection);

    const bool hasCurrent = view->selectionModel()->currentIndex().isValid();
    insertRowAction->setEnabled(hasCurrent);
    insertColumnAction->setEnabled(hasCurrent);

    if (hasCurrent) {
        view->closePersistentEditor(view->selectionModel()->currentIndex());

        const int row = view->selectionModel()->currentIndex().row();
        const int column = view->selectionModel()->currentIndex().column();
        if (view->selectionModel()->currentIndex().parent().isValid())
            statusBar()->showMessage(tr("Position: (%1,%2)").arg(row).arg(column));
        else
            statusBar()->showMessage(tr("Position: (%1,%2) in top level").arg(row).arg(column));
    }
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
    fileMenu->addAction(tr("&Save..."), QKeySequence::Save,
        this, [this]() { saveFile(); });
    fileMenu->addAction(tr("Save &As..."), QKeySequence::SaveAs,
        this, [this]() { saveFileAs(); });
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

    action = viewMenu->addAction(tr("&Tree Browser"), this, &MainWindow::toggleTreeBrowserDockWidget);
    action->setCheckable(true);
    action->setChecked(dockOptions() & AnimatedDocks);
    connect(action, &QAction::toggled, this, &MainWindow::toggleTreeBrowserDockWidget);;

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

    treeBrowserDockWidget = new QDockWidget();
    treeBrowserDockWidget->setObjectName("Object Browser");
    treeBrowserDockWidget->setWindowTitle("Object Browser");

    view = new QTreeView();
    view->setObjectName("view");
    view->setAlternatingRowColors(true);
    view->setSelectionBehavior(QAbstractItemView::SelectItems);
    view->setHorizontalScrollMode(QAbstractItemView::ScrollPerPixel);
    view->setAnimated(false);
    view->setAllColumnsShowFocus(true);

    TreeModel* model = new TreeModel({"AST"}, {}, this);
    view->setModel(model);
    treeBrowserDockWidget->setWidget(view);
    addDockWidget(Qt::RightDockWidgetArea, treeBrowserDockWidget);
    
    connect(view->selectionModel(), &QItemSelectionModel::selectionChanged,
        this, &MainWindow::updateActions);

    insertRowAction = new QAction(this);
    insertRowAction->setObjectName("insertRowAction");
    removeRowAction = new QAction(this);
    removeRowAction->setObjectName("removeRowAction");
    insertColumnAction = new QAction(this);
    insertColumnAction->setObjectName("insertColumnAction");
    removeColumnAction = new QAction(this);
    removeColumnAction->setObjectName("removeColumnAction");
    insertChildAction = new QAction(this);
    insertChildAction->setObjectName("insertChildAction");

    connect(insertRowAction, &QAction::triggered, this, &MainWindow::insertRow);
    connect(insertColumnAction, &QAction::triggered, this, &MainWindow::insertColumn);
    connect(removeRowAction, &QAction::triggered, this, &MainWindow::removeRow);
    connect(removeColumnAction, &QAction::triggered, this, &MainWindow::removeColumn);
    connect(insertChildAction, &QAction::triggered, this, &MainWindow::insertChild);

    updateActions();

    insertRow();

    //insertRow();
    tabifyDockWidget(treeBrowserDockWidget, projectDockWidget);
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

void MainWindow::toggleTreeBrowserDockWidget()
{
    auto toggleAction = treeBrowserDockWidget->toggleViewAction();

    treeBrowserDockWidget->addAction(toggleAction);
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
    using namespace weec;
    using namespace weec::lex;
    using namespace weec::parse;
    using namespace weec::interpreter;

    emit printed("Executing...");
    auto parseStart = chrono::system_clock::now();

    auto Result = Interpreter->Exec();

    if (Interpreter->Error.Code != wcInterpreterErrorCode::None)
    {
        emit printed("= = = Parsing Failed = = =");
        emit printed("Error code: " + string(to_string(int(Interpreter->Error.Code))) + "  " + to_string(Interpreter->Error.Code));
        return;
    }

    auto timeTaken = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - parseStart).count();
    emit printed("Execution Finished: " + to_string(timeTaken) + "ms");
    emit printed("Return: " + Interpreter->to_string(Result));
    emit printed("EAX: " + Interpreter->to_string(Interpreter->EAX));

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

ParserWorker::~ParserWorker()
{
}

void ParserWorker::process()
{
    using namespace std;
    using namespace weec;
    using namespace weec::lex;
    using namespace weec::parse;

    emit printed("Parsing...");
    auto parseStart = chrono::system_clock::now();
    wcTokenizer Tokenizer(Input);
    Output = wcParser(Tokenizer).Parse();
    auto timeTaken = chrono::duration_cast<chrono::milliseconds>(chrono::system_clock::now() - parseStart).count();

    if (Output.Error.Code != wcParserErrorCode::None)
    {
        emit printed("= = = Parsing Failed = = =");
        emit printed("Error code: " + string(to_string(int(Output.Error.Code))) + "  " + to_string(Output.Error.Code));
        emit printed(Output.Error.Token.to_string() + " (" + to_string(Output.Error.Token.StringToken.Line) + "," + to_string(Output.Error.Token.StringToken.Column) + ")");
        return;
    }

    //write to file

    emit printed("");
    emit printed("Build complete");
    emit printed("Time taken: " + to_string(timeTaken) + string("ms") + "\n");
    emit finished();
}
