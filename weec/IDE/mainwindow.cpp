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
    setupEditor();

    setupFileMenu();
    setupEditMenu();
    setupBuildMenu();
    setupProjectMenu();
    setupViewMenu();
    setupHelpMenu();
    setupDockWidgets();

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

void MainWindow::openFile()
{
    QString fileName = Filename.c_str();

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
    Build_RunSubMenu->setIcon(QIcon("../icons/running.png"));

    QThread* thread = new QThread();
    InterpreterWorker* worker = new InterpreterWorker(Parsed);
    worker->moveToThread(thread);

    //connect(worker, &InterpreterWorker::error, this, &InterpreterWorker::errorString);    
    connect(worker, &InterpreterWorker::printed, this, &MainWindow::printToOutput);
    connect(thread, &QThread::started, worker, &InterpreterWorker::process);
    connect(worker, &InterpreterWorker::finished, thread, &QThread::quit);
    connect(worker, &InterpreterWorker::finished, worker, &InterpreterWorker::deleteLater);
    connect(worker, &InterpreterWorker::finished, this, [this]() { InterpreterRunning = false; });
    connect(worker, &InterpreterWorker::finished, this, [this]() { Build_RunSubMenu->setIcon(QIcon("../icons/run.png")); });
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
    QToolBar* tb = addToolBar(tr("File Actions"));
    menuBar()->addMenu(fileMenu);

    const QIcon newIcon = QIcon("../icons/new.png");
    QAction* a = fileMenu->addAction(newIcon, tr("&New"), this, &MainWindow::newFile);
    tb->addAction(a);
    a->setPriority(QAction::LowPriority);
    a->setShortcut(QKeySequence::New);

    const QIcon openIcon = QIcon("../icons/open.png");
    a = fileMenu->addAction(openIcon, tr("&Open..."), this, &MainWindow::openFile);
    a->setShortcut(QKeySequence::Open);
    tb->addAction(a);

    tb->addSeparator();
    const QIcon saveIcon = QIcon("../icons/save.png");
    auto actionSave = fileMenu->addAction(saveIcon, tr("&Save"), this, &MainWindow::saveFile);
    actionSave->setShortcut(QKeySequence::Save);
    actionSave->setEnabled(true);
    tb->addAction(actionSave);

    const QIcon saveAsIcon = QIcon("../icons/save_as.png");
    auto actionSaveAs = fileMenu->addAction(saveAsIcon, tr("Save &As"), this, &MainWindow::saveFileAs);
    actionSaveAs->setShortcut(QKeySequence::SaveAs);
    actionSaveAs->setEnabled(true);
    tb->addAction(actionSaveAs);


    fileMenu->addAction(tr("E&xit"), QKeySequence::Quit,
        qApp, &QApplication::quit);
}

void MainWindow::setupEditMenu()
{
    QMenu* editMenu = new QMenu(tr("&Edit"), this);
    QToolBar* tb = addToolBar(tr("Edit Actions"));
    menuBar()->addMenu(editMenu);

    const QIcon undoIcon = QIcon("../icons/undo.png");
    actionUndo = editMenu->addAction(undoIcon, tr("&Undo"), editor, &QTextEdit::undo);
    actionUndo->setShortcut(QKeySequence::Undo);
    tb->addAction(actionUndo);

    const QIcon redoIcon = QIcon("../icons/redo.png");
    actionRedo = editMenu->addAction(redoIcon, tr("&Redo"), editor, &QTextEdit::redo);
    actionRedo->setPriority(QAction::LowPriority);
    actionRedo->setShortcut(QKeySequence::Redo);
    tb->addAction(actionRedo);
    editMenu->addSeparator();

    const QIcon cutIcon =  QIcon("../icons/cut.png");
    actionCut = editMenu->addAction(cutIcon, tr("Cu&t"), editor, &QTextEdit::cut);
    actionCut->setPriority(QAction::LowPriority);
    actionCut->setShortcut(QKeySequence::Cut);
    tb->addAction(actionCut);

    const QIcon copyIcon = QIcon("../icons/copy.png");
    actionCopy = editMenu->addAction(copyIcon, tr("&Copy"), editor, &QTextEdit::copy);
    actionCopy->setPriority(QAction::LowPriority);
    actionCopy->setShortcut(QKeySequence::Copy);
    tb->addAction(actionCopy);
    
    const QIcon pasteIcon = QIcon("../icons/paste.png");
    actionPaste = editMenu->addAction(pasteIcon, tr("&Paste"), editor, &QTextEdit::paste);
    actionPaste->setPriority(QAction::LowPriority);
    actionPaste->setShortcut(QKeySequence::Paste);
    tb->addAction(actionPaste);
    if (const QMimeData* md = QGuiApplication::clipboard()->mimeData())
        actionPaste->setEnabled(md->hasText());

}
void MainWindow::setupBuildMenu()
{
    QMenu* buildMenu = new QMenu(tr("&Build"), this);
    QToolBar* tb = addToolBar(tr("Build Actions"));
    menuBar()->addMenu(buildMenu);

    const QIcon buildIcon = QIcon("../icons/build.png");
    const QIcon runIcon = QIcon("../icons/run.png");
    Build_RunSubMenu = buildMenu->addAction(runIcon, tr("&Run"), QKeySequence(Qt::Key_F5), this, &MainWindow::buildAndRun);
    Build_BuildSubMenu = buildMenu->addAction(buildIcon, tr("&Build"), QKeySequence(Qt::Key_F7), this, &MainWindow::build);
    tb->addAction(Build_BuildSubMenu);
    tb->addAction(Build_RunSubMenu);
}


void MainWindow::setupViewMenu()
{
    viewMenu = new QMenu(tr("&View"), this);
    QToolBar* tb = addToolBar(tr("View Actions"));
    menuBar()->addMenu(viewMenu);

    const QIcon projectIcon = QIcon("../icons/project.png");
    View_ProjectSubMenu = viewMenu->addAction(projectIcon, tr("&Project"), this, &MainWindow::toggleProjectDockWidget);
    View_ProjectSubMenu->setCheckable(true);
    View_ProjectSubMenu->setChecked(dockOptions() & AnimatedDocks);
    tb->addAction(View_ProjectSubMenu);
    connect(View_ProjectSubMenu, &QAction::toggled, this, &MainWindow::toggleProjectDockWidget);


    const QIcon treeIcon = QIcon("../icons/tree_browser.png");
    View_TreeBrowserSubMenu = viewMenu->addAction(treeIcon, tr("&Tree Browser"), this, &MainWindow::toggleTreeBrowserDockWidget);
    View_TreeBrowserSubMenu->setCheckable(true);
    View_TreeBrowserSubMenu->setChecked(dockOptions() & AnimatedDocks);
    tb->addAction(View_TreeBrowserSubMenu);
    connect(View_TreeBrowserSubMenu, &QAction::toggled, this, &MainWindow::toggleTreeBrowserDockWidget);

    tb->addSeparator();
    viewMenu->addSeparator();

    const QIcon outputIcon = QIcon("../icons/output.png");
    View_OutputSubMenu = viewMenu->addAction(outputIcon, tr("&Output"), this, &MainWindow::toggleOutputDockWidget);
    View_OutputSubMenu->setCheckable(true);
    View_OutputSubMenu->setChecked(dockOptions() & AnimatedDocks);
    tb->addAction(View_OutputSubMenu);
    connect(View_OutputSubMenu, &QAction::toggled, this, &MainWindow::toggleOutputDockWidget);

    const QIcon buildIcon = QIcon("../icons/build2.png");
    View_BuildSubMenu = viewMenu->addAction(buildIcon, tr("&Build"), this, &MainWindow::toggleBuildDockWidget);
    View_BuildSubMenu->setCheckable(true);
    View_BuildSubMenu->setChecked(dockOptions() & AnimatedDocks);
    tb->addAction(View_BuildSubMenu);
    connect(View_BuildSubMenu, &QAction::toggled, this, &MainWindow::toggleBuildDockWidget);

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

void MainWindow::setupToolBar()
{



}

void MainWindow::setupDockWidgets()
{
    qRegisterMetaType<QDockWidget::DockWidgetFeatures>();

    outputDockWidget = new QDockWidget();
    outputDockWidget->setObjectName("Output");
    outputDockWidget->setWindowTitle("Output");
    outputDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    outputDockWidget->setWidget(outputTextEdit = new QTextEdit());
    addDockWidget(Qt::BottomDockWidgetArea, outputDockWidget);

    buildDockWidget = new QDockWidget();
    buildDockWidget->setObjectName("Build");
    buildDockWidget->setWindowTitle("Build");
    buildDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    buildDockWidget->setWidget(buildTextEdit = new QTextEdit());
    addDockWidget(Qt::BottomDockWidgetArea, buildDockWidget);

    tabifyDockWidget(outputDockWidget, buildDockWidget);

    projectDockWidget = new QDockWidget();
    projectDockWidget->setObjectName("Project");
    projectDockWidget->setWindowTitle("Project");
    projectDockWidget->setWidget(new QTextEdit);
    projectDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);
    addDockWidget(Qt::RightDockWidgetArea, projectDockWidget);
    auto projectDockWidgetAction = new QAction(projectDockWidget);
    projectDockWidgetAction->setObjectName("Project");

    treeBrowserDockWidget = new QDockWidget();
    treeBrowserDockWidget->setObjectName("Tree Browser");
    treeBrowserDockWidget->setWindowTitle("Tree Browser");
    treeBrowserDockWidget->setFeatures(QDockWidget::DockWidgetFloatable | QDockWidget::DockWidgetMovable);

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


    //insertRow();
    tabifyDockWidget(treeBrowserDockWidget, projectDockWidget);
}

void MainWindow::toggleProjectDockWidget()
{
    auto tva = projectDockWidget->toggleViewAction();

    if (!projectDockWidget->isVisible())
        if (View_ProjectSubMenu->isChecked())
        {
            projectDockWidget->show();
            return;
        }

    if (projectDockWidget->isVisible())
        if (!View_ProjectSubMenu->isChecked())
        {
            projectDockWidget->hide();
            return;
        }

}

void MainWindow::toggleOutputDockWidget()
{
    if (!outputDockWidget->isVisible())
        if (View_OutputSubMenu->isChecked())
        {
            outputDockWidget->show();
            return;
        }

    if (outputDockWidget->isVisible())
        if (!View_OutputSubMenu->isChecked())
        {
            outputDockWidget->hide();
            return;
        }
}

void MainWindow::toggleBuildDockWidget()
{
    if (!buildDockWidget->isVisible())
        if (View_BuildSubMenu->isChecked())
        {
            buildDockWidget->show();
            return;
        }

    if (buildDockWidget->isVisible())
        if (!View_BuildSubMenu->isChecked())
        {
            buildDockWidget->hide();
            return;
        }
}

void MainWindow::toggleTreeBrowserDockWidget()
{
    if (!treeBrowserDockWidget->isVisible())
        if (View_TreeBrowserSubMenu->isChecked())
        {
            treeBrowserDockWidget->show();
            return;
        }

    if (treeBrowserDockWidget->isVisible())
        if (!View_TreeBrowserSubMenu->isChecked())
        {
            treeBrowserDockWidget->hide();
            return;
        }
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
