// Copyright (C) 2016 The Qt Company Ltd.
// SPDX-License-Identifier: LicenseRef-Qt-Commercial OR BSD-3-Clause

#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "highlighter.h"
#include "weec.h"

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
class QInterpreter;

//! [0]
class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);

    std::string Filename;
    weec::parse::wcParseOutput Parsed;
    QInterpreter* Interpreter;

    void printToOutput(std::string);
    void printToBuild(std::string);

public slots:
    void about();
    void newFile();
    void openFile(const QString& path = QString());
    void build();
    void buildAndRun();
    void projectSettings();
    void toggleProjectDockWidget();  
    void toggleOutputDockWidget();
    void toggleObjectBrowserDockWidget();
protected:
    void setupEditor();
    void setupFileMenu();
    void setupBuildMenu();
    void setupProjectMenu();
    void setupViewMenu();
    void setupHelpMenu();
    
    void setupDockWidgets();

    void printTree(tree<weec::parse::wcParseNode> pTree)
    {
        auto it = pTree.begin();

        while (it != pTree.end())
        {
            std::string indent = "";
            for (int t = 0; t < pTree.depth(it); t++)
                indent += " - ";
            printToBuild(indent + it->Token.StringToken.Data + " (" + to_string(it->Type) + ")");
            it++;
        }
    }

    QList<ToolBar*> toolBars;
    QDockWidget* projectDockWidget, * buildDockWidget, *outputDockWidget, *objectBrowserDockWidget;
    QTextEdit* buildTextEdit,*outputTextEdit;

    QMenu* dockWidgetMenu;
    QList<QDockWidget*> extraDockWidgets;
    QMenu* viewMenu;

    QTextEdit *editor;
    Highlighter *highlighter;

};
//! [0]

class QInterpreter : public weec::interpreter::wcInterpreter
{
public:
    QInterpreter(weec::parse::wcParseOutput& _Input, MainWindow* _Win)
        : Win(_Win), wcInterpreter(_Input)
    {
        //Input = _Input;
        PC = _Input.AST.begin();
        Halt = false;
    }

    MainWindow* Win;

    void PrintFunc(std::string In, MainWindow* Win)
    {
        Win->printToOutput(In);
    }

    virtual void Print(std::any Value)
    {
        auto FindA = Win->Interpreter->SymbolTable.ImplTypes.GetByInternal(Value.type().name());
        if (FindA.Name == "" || FindA.Name == "void")
        {
            PrintFunc(">", Win);
            return;
        }

        if (FindA.Name == "int")
            PrintFunc(">" + std::to_string(any_cast<int>(Value)) + "\n", Win);
        else if (FindA.Name == "unsigned int")
            PrintFunc(">" + std::to_string(any_cast<unsigned int>(Value)) + "\n", Win);
        else if (FindA.Name == "float")
            PrintFunc(">" + std::to_string(any_cast<float>(Value)) + "\n", Win);
        else if (FindA.Name == "double")
            PrintFunc(">" + std::to_string(any_cast<double>(Value)) + "\n", Win);
        else if (FindA.Name == "bool")
            PrintFunc(">" + std::to_string(any_cast<bool>(Value)) + "\n", Win);
        else if (FindA.Name == "string")
            PrintFunc(">" + any_cast<std::string>(Value) + "\n", Win);
    }

    std::any Exec()
    {
        if (Input.Error.Code != weec::parse::wcParserErrorCode::None)
            return weec::interpreter::wcInterpreterError(weec::interpreter::wcInterpreterErrorCode::BadInput, *Input.AST.begin());

        while (PC != Input.AST.end() && PC != nullptr && !Halt)
        {
            auto t = PC->Type;
            switch (PC->Type)
            {
            case weec::parse::wcParseNodeType::Statement:
                ExecStatement();
                break;

            case weec::parse::wcParseNodeType::Block:
                ExecBlock();
                break;

            case weec::parse::wcParseNodeType::Head:
                //push global stack frame
                SymbolTable.StackFrames.push(weec::interpreter::wcInterpreterStackFrame("$g", PC, std::vector<std::any>()));
                PC++;
                break;

            default:
                Error.Node = *PC;
                Error.Code = weec::interpreter::wcInterpreterErrorCode::InvalidNode;
                Halt = true;
                break;
            }

            Win->update();
            Win->repaint();
        }

        return Return;
    }
};
#endif // MAINWINDOW_H
