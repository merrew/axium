#pragma once

#ifndef ANALYSISPANEL_H
#define ANALYSISPANEL_H

#include "Fonts.h"
#include "panel.h"
#include <QScrollBar>
#include <QScrollArea>
#include "FuncGL.h"

#include <QTextEdit>
#include <QLineEdit>
#include <QMessageBox>
#include <QRadioButton>
#include <QButtonGroup>
class AnalysisFunction
{
public:
    QWidget* localWidget;
    QVBoxLayout* localLayout;

    QLabel* title;
    QTextEdit* result;

    AnalysisFunction(QString title, QWidget* parent, QVBoxLayout* parent_layout)
    {
        localWidget = new QWidget(parent);
        localWidget->setStyleSheet(
            "QWidget {"
            "  border: none;"
            "}"
            );
        localLayout = new QVBoxLayout(localWidget);

        this->title = new QLabel(title, localWidget);
        this->title->setStyleSheet("color: " + white + "; font: 14px; font-family: " + GlobalFont + "; "
                                                                                                    "border: 1px solid " +  lightgray + ";");
        localLayout->addWidget(this->title);

        result = new QTextEdit(localWidget);
        result->setFixedHeight(40);
        result->setReadOnly(true);
        result->setPlainText("");      // пустой текст
        result->setMaximumWidth(300);  // ограничение ширины
        result->setStyleSheet("font: 14px; font-family: " + GlobalFont + "; " + "background-color: " + gray + "; color: " + white + "; border: 1px solid " + lightgray + ";");
        this->result->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        this->result->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);

        localLayout->addWidget(result);

        parent_layout->addWidget(this->localWidget);
    }
private:
};

class AnalysisPanel : public Panel
{
    Q_OBJECT
public:
    AnalysisPanel(QMainWindow* window);
    void setActiveFunction(FuncGL* f);

    std::unordered_map<std::string, AnalysisFunction*> analysis_functions;
    FuncGL* activeFunc = nullptr;
    QLineEdit* aEdit;
    QLineEdit* bEdit;

    QButtonGroup* integralGroup;
    QRadioButton* rectBtn;
    QRadioButton* trapBtn;
    QRadioButton* simpBtn;
    QRadioButton* gaussBtn;

private slots:
    void calculate();
};

#endif // ANALYSISPANEL_H
