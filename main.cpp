#pragma once

#include "Fonts.h"
#include "panel.h"
#include "toolspanel.h"
#include "analysispanel.h"
#include "Graph_function.h"
#include "OpenGLContainer.h"
#include <QSurfaceFormat>
#include "parser/FuncParse.h"

#include "OpenGLContainer.h"

int main(int argc, char *argv[])
{
    try {
    QSurfaceFormat fmt;
    fmt.setVersion(3, 3); // OpenGL 3.3 (современная стабильная версия)
    fmt.setProfile(QSurfaceFormat::CoreProfile); // современный профиль (без устаревших функций)
    fmt.setDepthBufferSize(24); // разрядность буфера глубины (расстояние от камеры до каждой точки)
    fmt.setStencilBufferSize(8); // для спец.эффектов
    fmt.setSamples(4); // сглаживаиние (среднее качество сглаживания, но высокая производитлеьность)
    QSurfaceFormat::setDefaultFormat(fmt);

    QApplication app(argc, argv);

    // Загружаем шрифты
    loadFonts();

    // Окно
    QMainWindow window;
    window.setWindowTitle("Axium");
    window.resize(1400, 800);
    window.setStyleSheet("background-color: " + gray + "; border: 1px solid " + gray + ";");
    window.setWindowIcon(QIcon(":/icon_prefix/icon.png"));

    // Панель инструментов
    ToolsPanel *tool_panel = new ToolsPanel(&window);
    window.addDockWidget(Qt::LeftDockWidgetArea, tool_panel);

    // Панель анализа
    Panel *analysis_panel = new AnalysisPanel(&window);
    window.addDockWidget(Qt::RightDockWidgetArea, analysis_panel);

    // Задаём стартовую ширину панелей 300 пикселей
    QList<QDockWidget*> docks;
    docks.append(tool_panel);
    docks.append(analysis_panel);
    window.resizeDocks(docks, QList<int>() << 300 << 300, Qt::Horizontal);

    // Окно графика
    Graph_function *graph_f = new Graph_function();
    OpenGLContainer* central = new OpenGLContainer(graph_f, &window);
    graph_f->setParent(central);

    graph_f->setGeometry(0, 0, 600, 600);
    graph_f->show();
    // Прикрепляем панели к кнопке (закрытие при нажатии ПКМ)
    QObject::connect(graph_f, &Graph_function::rightClicked, [&tool_panel, &analysis_panel, &graph_f]() {
        tool_panel->setVisible(!tool_panel->isVisible());
        analysis_panel->setVisible(!analysis_panel->isVisible());

        //graph_f->setDefaultFont();
    });
    QObject::connect(analysis_panel, &Panel::resized, [&graph_f, &central](QSize oldSize, QSize newSize){
        int size = qMax(central->width(), central->height());
        int x = (central->width() - size) / 2;
        int y = (central->height() - size) / 2;
        graph_f->setGeometry(x, y, size, size);
    });
    QObject::connect(tool_panel, &Panel::resized, [&graph_f, &central](QSize oldSize, QSize newSize){
        int size = qMax(central->width(), central->height());
        int x = (central->width() - size) / 2;
        int y = (central->height() - size) / 2;
        graph_f->setGeometry(x, y, size, size);
    });
    window.setCentralWidget(central);

    // Для отрисовки графика функции
    bool ok = QObject::connect(tool_panel, &ToolsPanel::addFunc,
                     graph_f, &Graph_function::addFunc);
    qDebug() << "connect functionAdded -> addFunc:" << ok;

    QObject::connect(tool_panel, &ToolsPanel::updateFunc,
                     graph_f, &Graph_function::updateFunc);
    QObject::connect(tool_panel, &ToolsPanel::removeFunc,
                     graph_f, &Graph_function::removeFunc);
    QObject::connect(tool_panel, &ToolsPanel::updateFuncColor,
                     graph_f, &Graph_function::updateFuncColor);
    // Установка курсора-крестика для OpenGL окна
    graph_f->setCursor(	Qt::CrossCursor);

    window.show();
    return app.exec();
    }
    catch (const std::exception& e) {
        std::cerr << "EXCEPTION: " << e.what() << std::endl;
    }
    catch (...) {
        std::cerr << "UNKNOWN EXCEPTION!" << std::endl;
    }

    return -1;
}
