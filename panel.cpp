#pragma once

#include "panel.h"

void Panel::resizeEvent(QResizeEvent* event)
{
    emit resized(event->oldSize(), event->size());
    QDockWidget::resizeEvent(event);
}

Panel::Panel(QString text, QMainWindow *parent, Qt::DockWidgetArea area)
    : QDockWidget(text, parent)
{
    // Поведение панели - закрытие и перемещение
    setFeatures(QDockWidget::DockWidgetClosable |
                QDockWidget::DockWidgetMovable);

    setAllowedAreas(area); // разрешённое положение панели
    this->setMinimumWidth(300);

    // Заголовок
    QWidget* titleBar = new QWidget(this);
    titleBar->setStyleSheet("background-color: " + gray + "; border: none; font-weight: bold");
    setTitleBarWidget(titleBar);

    // Компоновщик заголовка
    QHBoxLayout *title_layout = new QHBoxLayout(titleBar);
    title_layout->setContentsMargins(0, 30, 0, 30);

    QLabel* titleLabel = new QLabel(text, titleBar);
    titleLabel->setStyleSheet("color: white; font: 18pt;");
    title_layout->addWidget(titleLabel, 1, Qt::AlignCenter);

    // Контейнер элементов внутри скролла
    QWidget* content = new QWidget(this);

    vLayout = new QVBoxLayout(content);
    vLayout->setContentsMargins(10,20,10,20);
    vLayout->setSpacing(10);


    content->setStyleSheet(R"(
        QWidget {
            background-color:)" + lightgray + R"(;
            border: 1px solid )" + lightgray + R"(;
            border-radius: 8;
            color: white;
        }

    )");

    // Скролл
    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidget(content);

    QScrollBar* vScroll = scroll->verticalScrollBar();
    vScroll->setStyleSheet(R"(
    QScrollBar:vertical {
        border: none;
        background: )" + light_lightgray + R"(;
        width: 5px;
        border-radius: 2px;
    }
    QScrollBar::handle:vertical {
        background: )" + blue + R"(;
        min-height: 20px;
        border-radius: 2px;
    }
    QScrollBar::handle:vertical:hover {
        background: )" + blue_hover + R"(;
    }
    QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {
        height: 0px;
    }
    QScrollBar::add-page:vertical, QScrollBar::sub-page:vertical {
        background: none;
    }
)");

    // Главный контейнер класса
    container = new QWidget(this);
    QVBoxLayout* mainLayout = new QVBoxLayout(container);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(scroll);
    setWidget(container);
}
