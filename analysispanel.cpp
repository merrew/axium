#include "analysispanel.h"

void AnalysisPanel::setActiveFunction(FuncGL* f)
{
    activeFunc = f;
    qDebug() << f;
}

AnalysisPanel::AnalysisPanel(QMainWindow* window) : Panel("АНАЛИЗ", window, Qt::RightDockWidgetArea)
{
    // Интервал интегрирования
    QWidget* intervalWidget = new QWidget(container);
    QHBoxLayout* intervalLayout = new QHBoxLayout(intervalWidget);

    QLabel* intervalLabel = new QLabel("Интервал: [", intervalWidget);
    intervalLabel->setStyleSheet("color: " + white + "; font: 16px;");

    aEdit = new QLineEdit(intervalWidget);
    aEdit->setPlaceholderText("a");
    aEdit->setAlignment(Qt::AlignCenter);
    aEdit->setStyleSheet("background-color: " + gray + "; font: 16px;");

    QLabel* separator = new QLabel(";", intervalWidget);
    intervalLabel->setStyleSheet("color: " + white + "; font: 16px;");

    bEdit = new QLineEdit(intervalWidget);
    bEdit->setPlaceholderText("b");
    bEdit->setAlignment(Qt::AlignCenter);
    bEdit->setStyleSheet("background-color: " + gray + "; font: 16px;");

    QLabel* end = new QLabel("]", intervalWidget);
    intervalLabel->setStyleSheet("color: " + white + "; font: 16px;");

    intervalLayout->addWidget(intervalLabel);
    intervalLayout->addWidget(aEdit);
    intervalLayout->addWidget(separator);
    intervalLayout->addWidget(bEdit);
    intervalLayout->addWidget(end);

    vLayout->addWidget(intervalWidget);


    // Кнопка вычисления
    QPushButton* calcButton = new QPushButton("Рассчитать", container);
    calcButton->setFixedSize(275, 30);

    calcButton->setStyleSheet(
        "QPushButton {"
        "background-color: " + blue + ";"
                 "color: white;"
                 "font: 14px; font-family: " + GlobalFont + ";"
                       "border-radius: 4px;"
                       "}"
                       "QPushButton:hover {"
                       "background-color: " + blue_hover + ";"
                       "}"
        );

    connect(calcButton, &QPushButton::clicked,
            this, &AnalysisPanel::calculate);
    vLayout->addWidget(calcButton);

    QString titles[3] = {"Основные свойства", "Дифференциальный анализ", "Интегральный анализ"};
    // QWidget* analysis_scrolls[3];
    QVBoxLayout* analysis_layouts[3];
    QWidget* analysis_widgets[3];

    for (int i = 0; i < 3; i++)
    {
        // заголовок + скролл
        QWidget* sectionWidget = new QWidget(container);
        QVBoxLayout* sectionLayout = new QVBoxLayout(sectionWidget);

        // заголовок секции
        QLabel* title = new QLabel(titles[i], sectionWidget);
        title->setStyleSheet(
            "color: " + white +
            "; font: 18px; font-family: " + GlobalBoldFont +
            "; border: 1px solid " + lightgray + ";"
            );

        // ScrollArea
        //analysis_scrolls[i] = new QWidget(sectionWidget);
        //analysis_scrolls[i]->setWidgetResizable(true);
        //analysis_scrolls[i]->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
        //analysis_scrolls[i]->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);

        // Контент скролла
        analysis_widgets[i] = new QWidget(sectionWidget);
        analysis_widgets[i]->setStyleSheet(
            "QWidget {"
            "  border: 1px solid " + light_lightgray + ";"
                                "  border-radius: 6px;"
                                "}"
            );
        analysis_layouts[i] = new QVBoxLayout(analysis_widgets[i]);
        /*
        analysis_scrolls[i]->setWidget(analysis_widgets[i]);

        QScrollBar* vScroll = analysis_scrolls[i]->verticalScrollBar();
        vScroll->setStyleSheet(R"(
        QScrollBar:vertical {
            border: none;
            background: )" + light_lightgray + R"(;
            width: 5px;
            border-radius: 1px;
        }
        QScrollBar::handle:vertical {
            background: )" + blue + R"(;
            min-height: 10px;
            border-radius: 1px;
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
*/
        sectionLayout->addWidget(title);
        sectionLayout->addWidget(analysis_widgets[i]);

        vLayout->addWidget(sectionWidget);
    }


    analysis_functions.emplace(
        "Область определения",
        new AnalysisFunction("Область определения: ", analysis_widgets[0], analysis_layouts[0])
        );
    analysis_functions.emplace(
        "Область значения",
        new AnalysisFunction("Область значения: ", analysis_widgets[0], analysis_layouts[0])
        );
    analysis_functions.emplace(
        "Точки нулей функции",
        new AnalysisFunction("Точки нулей функции: ", analysis_widgets[0], analysis_layouts[0])
        );
    analysis_functions.emplace(
        "Точки разрыва",
        new AnalysisFunction("Точки разрыва: ", analysis_widgets[0], analysis_layouts[0])
        );

    /*
    analysis_functions.emplace(
        "Производная в точке",
        new AnalysisFunction("Производная в точке: ", analysis_widgets[1], analysis_layouts[1])
        );
*/
    analysis_functions.emplace(
        "Экстремумы",
        new AnalysisFunction("Экстремумы: ", analysis_widgets[1], analysis_layouts[1])
        );
    analysis_functions.emplace(
        "Интервалы монотонности",
        new AnalysisFunction("Интервалы монотонности: ", analysis_widgets[1], analysis_layouts[1])
        );
    analysis_functions.emplace(
        "Точки перегиба",
        new AnalysisFunction("Точки перегиба: ", analysis_widgets[1], analysis_layouts[1])
        );
    analysis_functions.emplace(
        "Области выпуклости",
        new AnalysisFunction("Области выпуклости: ", analysis_widgets[1], analysis_layouts[1])
        );

    analysis_functions.emplace(
        "Интеграл (прямоугольники)",
        new AnalysisFunction("Интеграл (прямоугольники): ", analysis_widgets[2], analysis_layouts[2])
        );

    analysis_functions.emplace(
        "Интеграл (трапеции)",
        new AnalysisFunction("Интеграл (трапеции): ", analysis_widgets[2], analysis_layouts[2])
        );

    analysis_functions.emplace(
        "Интеграл (Симпсон)",
        new AnalysisFunction("Интеграл (Симпсон): ", analysis_widgets[2], analysis_layouts[2])
        );

    analysis_functions.emplace(
        "Интеграл (Гаусс)",
        new AnalysisFunction("Интеграл (Гаусс): ", analysis_widgets[2], analysis_layouts[2])
        );
    analysis_functions.emplace(
        "Длина кривой на отрезке",
        new AnalysisFunction("Длина кривой на отрезке: ", analysis_widgets[2], analysis_layouts[2])
        );

    setWidget(container);
}

void AnalysisPanel::calculate()
{
    if(!activeFunc)
    {
        QMessageBox msg;
        msg.setWindowTitle("Ошибка");
        msg.setText("Не выбрана ни одна из функций (для выбора кликните по графику соответствующей функции)");

        msg.setStyleSheet(
            "QLabel{ color: " + white + "; }"
                                        "QPushButton{background-color: " + blue + "; color: " + white + ";}"
                                           "QPushButton:hover {background-color: " + blue_hover + ";}"
                           "QMessageBox{ background-color: " + gray + "; }"
            );

        msg.exec();
        return;
    }

    bool okA, okB;
    double a = aEdit->text().toDouble(&okA);
    double b = bEdit->text().toDouble(&okB);

    if (!okA || !okB) {
        QMessageBox msg;
        msg.setWindowTitle("Ошибка");
        msg.setText("Введите корректные числа для a и b!");

        msg.setStyleSheet(
            "QLabel{ color: " + white + "; }"
            "QPushButton{background-color: " + blue + "; color: " + white + ";}"
            "QPushButton:hover {background-color: " + blue_hover + ";}"
            "QMessageBox{ background-color: " + gray + "; }"
        );

        msg.exec();
        return;
    }

    if (a >= b) {
        QMessageBox msg;
        msg.setWindowTitle("Ошибка");
        msg.setText("Нижняя граница a должна быть меньше верхней границы b!");

        msg.setStyleSheet(
            "QLabel{ color: " + white + "; }"
            "QPushButton{background-color: " + blue + "; color: " + white + ";}"
            "QPushButton:hover {background-color: " + blue_hover + ";}"
            "QMessageBox{ background-color: " + gray + "; }"
        );

        msg.exec();
        return;
    }

    auto extrema = activeFunc->findExtrema(a, b,
                          [this](double x, double& y, double h) {
                              return activeFunc->eval(x, y, h);
                          },
                          [this](double x, double& d, double h) {
                              return activeFunc->d_dx(x, d, h);
                          });

    auto nulls = activeFunc->findNulls(a, b,
                        [this](double x, double& y, double h) {
                            return activeFunc->eval(x, y, h);
                        },
                        [this](double x, double& d, double h) {
                            return activeFunc->d_dx(x, d, h);
                        });

    auto break_points = activeFunc->findBreakPoints(a, b,
                              [this](double x, double& y, double h) {
                                  return activeFunc->eval(x, y, h);
                              },
                              [this](double x, double& d, double h) {
                                  return activeFunc->d_dx(x, d, h);
                              });

    auto in_mon = activeFunc->findIntervalsMono(a, b,
                                    [this](double x, double& y, double h) {
                                        return activeFunc->eval(x, y, h);
                                    },
                                    [this](double x, double& d, double h) {
                                        return activeFunc->d_dx(x, d, h);
                                    });

    auto D = activeFunc->find_D(a, b,
                    [this](double x, double& y, double h) {
                        return activeFunc->eval(x, y, h);
                    },
                    [this](double x, double& d, double h) {
                        return activeFunc->d_dx(x, d, h);
                    });

    auto E = activeFunc->find_E(a, b,
                    [this](double x, double& y, double h) {
                        return activeFunc->eval(x, y, h);
                    },
                    [this](double x, double& d, double h) {
                        return activeFunc->d_dx(x, d, h);
                    });

    auto ex2 = activeFunc->findExtrema(a, b,
                           [this](double x, double& y, double h) {
                               return activeFunc->d_dx(x, y, h);
                           },
                           [this](double x, double& d, double h) {
                               return activeFunc->d2_dx2(x, d, h);
                           });

    auto crit2_intervals = activeFunc->findIntervalsMono(a, b,
                                             [this](double x, double& y, double h) {
                                                 return activeFunc->d_dx(x, y, h);
                                             },
                                             [this](double x, double& d, double h) {
                                                 return activeFunc->d2_dx2(x, d, h);
                                             });

    double rect = activeFunc->rectangle_integrate(a,b);
    double trap = activeFunc->trapezoid_integrate(a,b);
    double simp = activeFunc->simpson_integrate(a,b);
    double gauss = activeFunc->gauss_composite_integrate(a,b);
    double length = activeFunc->curve_length(a,b);

    // Вывод
    // Экстремумы
    QString maximaText;
    QString minimaText;

    for (auto& e : extrema)
    {
        QString pointStr = "(" + QString::number(e.x, 'f', 3) + "; " + QString::number(e.y, 'f', 3) + ")";
        if (e.is_max)
            maximaText += pointStr + "  ";
        else
            minimaText += pointStr + "  ";
    }

    QString finalText;
    if (!maximaText.isEmpty())
        finalText += "Максимумы: " + maximaText + "\n";
    if (!minimaText.isEmpty())
        finalText += "Минимумы: " + minimaText + "\n";

    analysis_functions["Экстремумы"]->result->setText(finalText);

    // Нули
    finalText = "";
    for (auto& n : nulls)
    {
        finalText += "(" + QString::number(n.rx(), 'f', 3) + "; " + QString::number(n.ry(), 'f', 3) + ")";
    }
    analysis_functions["Точки нулей функции"]->result->setText(finalText);

    // Точки разрыва
    finalText = "";
    for (auto& bp : break_points)
    {
        finalText += QString::number(bp.x, 'f', 3) + "; ";
    }
    analysis_functions["Точки разрыва"]->result->setText(finalText);

    // Интервалы монотонности
    QString increasingText;
    QString decreasingText;

    for (auto& im : in_mon)
    {
        QString pointStr = "(" + QString::number(im.start, 'f', 3) + "; " + QString::number(im.end, 'f', 3) + ")";
        if (im.type == IntervalMonoType::Increasing)
            increasingText += pointStr + "  ";
        else
            decreasingText += pointStr + "  ";
    }

    finalText = "";
    if (!increasingText.isEmpty())
        finalText += "Промежутки возрастания: " + increasingText + "\n";
    if (!decreasingText.isEmpty())
        finalText += "Промежутки убывания: " + decreasingText + "\n";

    analysis_functions["Интервалы монотонности"]->result->setText(finalText);

    // Область определения
    finalText = "";
    for (auto& d : D)
    {
        finalText += "(" + QString::number(d.first, 'f', 3) + "; " + QString::number(d.second, 'f', 3) + ") U ";
    }
    analysis_functions["Область определения"]->result->setText(finalText);

    // Область значения
    finalText = "";
    for (auto& e : E)
    {
        finalText += "(" + QString::number(e.first, 'f', 3) + "; " + QString::number(e.second, 'f', 3) + ") U ";
    }
    analysis_functions["Область значения"]->result->setText(finalText);

    // Точки перегиба
    maximaText = "";
    minimaText = "";

    for (auto& e : ex2)
    {
        QString pointStr = "(" + QString::number(e.x, 'f', 3) + "; " + QString::number(e.y, 'f', 3) + ")";
        if (e.is_max)
            maximaText += pointStr + "  ";
        else
            minimaText += pointStr + "  ";
    }

    finalText = "";
    if (!maximaText.isEmpty())
        finalText += "Восходящие точки перегиба: " + maximaText + "\n";
    if (!minimaText.isEmpty())
        finalText += "Нисходящие точки перегиба: " + minimaText + "\n";
    analysis_functions["Точки перегиба"]->result->setText(finalText);

    // Области выпуклости
    increasingText = "";
    decreasingText = "";

    for (auto& ci : crit2_intervals)
    {
        QString pointStr = "(" + QString::number(ci.start, 'f', 3) + "; " + QString::number(ci.end, 'f', 3) + ")";
        if (ci.type == IntervalMonoType::Increasing)
            increasingText += pointStr + "  ";
        else
            decreasingText += pointStr + "  ";
    }

    finalText = "";
    if (!increasingText.isEmpty())
        finalText += "Промежутки вогнутости: " + increasingText + "\n";
    if (!decreasingText.isEmpty())
        finalText += "Промежутки выпуклости: " + decreasingText + "\n";
    analysis_functions["Области выпуклости"]->result->setText(finalText);

    // Интегрирование
    analysis_functions["Интеграл (прямоугольники)"]->result
        ->setText(QString::number(rect));

    analysis_functions["Интеграл (трапеции)"]->result
        ->setText(QString::number(trap));

    analysis_functions["Интеграл (Симпсон)"]->result
        ->setText(QString::number(simp));

    analysis_functions["Интеграл (Гаусс)"]->result
        ->setText(QString::number(gauss));

    analysis_functions["Длина кривой на отрезке"]->result
        ->setText(QString::number(length));
}
