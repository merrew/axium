#include "mathoperators.h"

// Переключатели кнопок (кружочки)
CircleButton::CircleButton(QWidget* parent)
    : QPushButton(parent), m_active(false)
{
    setFixedSize(15, 15);
    setStyleSheet("QPushButton {"
                  "background:" + lightgray + "; "
                                "border: none;}"
                                "QPushButton:hover {"
                                "background-color: " + blue_hover + ";"
                                 "}");
}

void CircleButton::SetActive(bool on)
{
    m_active = on;
    if (m_active) setFixedSize(20, 20);
    else setFixedSize(15, 15);

    update(); // перерисовка кнопки
}

void CircleButton::paintEvent(QPaintEvent* event)
{
    // Кастомизируем кнопку под круг
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing); // сглаживаем края

    QColor fill = m_active ? QColor(green) : QColor(blue);
    if (underMouse()) {
        fill = m_active ? QColor(green_hover) : QColor(blue_hover); // цвет при наведении мыши
    }
    p.setBrush(fill); // ставим заливку фона
    p.setPen(Qt::NoPen); // убираем контур

    int r = width()/2;
    // Рисуем круг в центре кнопки, радиусом чуть меньше половины ее ширины
    p.drawEllipse(rect().center(), r-2, r-2);
}

Page::Page(const QStringList &operators, QWidget* parent)
    :QWidget(parent)
{
    QWidget* container = new QWidget;

    QGridLayout* grid = new QGridLayout(container);
    int row = 0, col = 0;
    int columns = 3;

    grid->setContentsMargins(0,0,0,0);
    grid->setVerticalSpacing(4);   // расстояние между строками
    grid->setHorizontalSpacing(4); // расстояние между столбцами


    for (const QString &name : operators)
    {
        QPushButton* b = new QPushButton(name);
        b->setFocusPolicy(Qt::NoFocus);
        b->setFixedHeight(36);
        b->setMinimumWidth(60);
        b->setStyleSheet("QPushButton {font: bold 16px; "
                         "color: " + white + ";"
                                   "font-family: " + MathFont + "; "
                                      "background: " + light_lightgray + "; "
                                             "border-radius: 10px;}"
                                             "QPushButton:hover {background: " + blue + ";}"); // цвет при наведении мыши

        // При нажатии на кнопку генерируется сигнал operatorClicked(name)
        connect(b, &QPushButton::clicked, this, [this, name]() { emit operatorClicked(name); });

        grid->addWidget(b, row, col);
        if (++col >= columns) {
            col = 0;
            ++row;
        }
    }

    QScrollArea* scroll = new QScrollArea;
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setWidgetResizable(true);
    scroll->setWidget(container);

    page = new QWidget(parent);
    // Чтобы панель не уменьшалась с добавлением новых функций
    page->setMinimumHeight(150);
    page->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Fixed);

    QVBoxLayout* lay = new QVBoxLayout(page);
    lay->setContentsMargins(0,0,0,0);
    lay->addWidget(scroll);

}


