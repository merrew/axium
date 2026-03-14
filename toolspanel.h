#pragma once
#ifndef TOOLSPANEL_H
#define TOOLSPANEL_H

#include <QVector>
#include <list>
#include <QMouseEvent>

#include "panel.h"
#include "mathoperators.h"
#include "functions.h"

#include "jkqtmathtext/jkqtmathtextlabel.h"
#include "jkqtmathtext/jkqtmathtext.h"

#include <QMap>
// Панель "Функции"
class ToolsPanel: public Panel
{
    Q_OBJECT
public:
    int next_id = 0;
    ToolsPanel(QMainWindow* window);
    void AddFunc();
private:

    QVector<CircleButton*> circleButtons;
    QStackedWidget *stack_pages = nullptr;
    std::list<Functions*> Func;
    Functions* active_Func = nullptr;
    QWidget* addButtonContainer;
    //int Func_size = 3;

signals:
    void addFunc(const QString& expr, const QColor& color);
    void updateFunc(int index, const QString& expr);
    void updateFuncColor(int index, const QColor& color);
    void removeFunc(int index);
    // void activeFunctionChanged(const QString& text);
    // void colorFunctionChanged(QColor newColor);
};



#endif // TOOLSPANEL_H
