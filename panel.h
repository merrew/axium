#ifndef PANEL_H
#define PANEL_H

#include <QDockWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QMouseEvent>
#include <QMainWindow>
#include <QWidget>
#include <QString>
#include <QLabel>
#include <QScrollArea>
#include <QScrollBar>

#include "Colors.h"

// Класс стандартной панели
class Panel : public QDockWidget {
    Q_OBJECT
protected:
    QWidget* container;     // Контейнер для содержимого
    QVBoxLayout *vLayout; // Расположение

    void resizeEvent(QResizeEvent* event) override;

public:
    Panel(QString text, QMainWindow *parent, Qt::DockWidgetArea area);
signals:
    void resized(QSize oldSize, QSize newSize);
};

#endif // PANEL_H
