#include "functions.h"

Functions::Functions(QWidget* parent, int ind): QWidget(parent), id(ind)
{
    QWidget* content = new QWidget(parent);
    QVBoxLayout* lay = new QVBoxLayout(content);
    lay->setContentsMargins(0,0,0,20);
    lay->setSpacing(0);

    // Кнопка выбора цвета + метка
    QWidget* funcLabelContainer = new QWidget(content);

    QHBoxLayout* funcLabelLay = new QHBoxLayout(funcLabelContainer);
    funcLabelLay->setContentsMargins(0, 0, 0, 0);

    // Кнопка выбора цвета
    QPushButton* colorBtn = new QPushButton(funcLabelContainer);
    colorBtn->setFixedSize(14, 14);
    colorBtn->setStyleSheet(
        "QPushButton {"
        "border-radius: 7px;"
        "background-color: " + lightgray + ";"
                      "border: 2px solid " + green + ";"
                  "}"
                  "QPushButton:hover {"
                  "background-color: " + green + ";"
                  "}"
        );
    // Метка
    // !

    funcLabel = new JKQTMathTextLabel(funcLabelContainer);
    funcLabel->setMath("$\\mathrm{\\color{" + white + "}{f(x)}}$");

    //funcLabel->setFont(QFont(MathFont));
    // !
    //QLabel* funcLabel = new QLabel("f(x) = sin(x)", funcLabelContainer);
    //funcLabel->setStyleSheet("color: " + white + "; font: 20px; font-family: " + MathFont + "; "
    //                       "border: 2px solid " +  lightgray + ";");

    funcLabelLay->addWidget(colorBtn);
    funcLabelLay->addWidget(funcLabel);

    lay->addWidget(funcLabelContainer);

    // Подключаем кнопку к QLabelDialog
    QObject::connect(colorBtn, &QPushButton::clicked, [this, colorBtn, funcLabelContainer]()
                     {
                         QColorDialog dlg(funcLabelContainer);
                         dlg.setWindowTitle("Выберите цвет функции");

                         // Настраиваем палитру окна диалога
                         dlg.setStyleSheet(QString(R"(

            QColorDialog {
                background-color: )" + gray + R"(;
                color: )" + white + R"(;
                font-family: )" + GlobalFont.front() + R"(;
            }
            QPushButton {
                background-color: )" + blue + R"(;
                color: )" + white + R"(;
                border-radius: 8px;
                font-family: )" + GlobalFont.front() + R"(;
                padding: 6px 6px;
            }
            QPushButton:hover {
                background-color: )" + blue_hover + R"(;
            }
            QLabel {
                font-family: )" + GlobalFont.front() + R"(;
                color: )" + white + R"(;
                background-color: )" + lightgray + R"(;
            }
            QSpinBox {
                font-family: )" + GlobalFont.front() + R"(;
                color: )" + white + R"(;
                background-color: )" + lightgray + R"(;
            }
            QLineEdit {
                font-family: )" + GlobalFont.front() + R"(;
                color: )" + white + R"(;
                background-color: )" + lightgray + R"(;
            }
        )"));

                         if (dlg.exec() == QDialog::Accepted) {
                             QColor newColor = dlg.selectedColor();
                             if (newColor.isValid()) {
                                 colorBtn->setStyleSheet(
                                     "QPushButton {"
                                     "border-radius: 7px;"
                                     "color: " + white + ";"
                                     "background-color: " + lightgray + ";"
                                                   "border: 2px solid " + newColor.name() + ";"
                                                         "}"
                                                         "QPushButton:hover {"
                                                         "background-color: " + newColor.name() + ";"
                                                         "}"
                                     );
                                 this->Color = newColor;
                                 // смена цвета при отрисовке
                                 emit colorFunctionChanged(newColor);
                             }
                         }
                     });

    // Кнопка удаления функции
    QPushButton* delBtn = new QPushButton(funcLabelContainer);
    delBtn->setIcon(QIcon(":/icon_prefix/delete.png")); // путь к картинке в ресурсах
    delBtn->setIconSize(QSize(20, 20));           // размер иконки
    delBtn->setFixedSize(27, 27);
    delBtn->setStyleSheet(
        "QPushButton {"
        "border-radius: 7px;"
        "background-color: " + lightgray + ";"
                      "border: 2px solid " + lightgray + ";"
                      "}"
                      "QPushButton:hover {"
                      "background-color: " + gray + ";"
                 "border: 2px solid " + gray + ";"
                 "}"
        );
    // При нажатии на кнопку генерируется сигнал needToDelete(id)
    connect(delBtn, &QPushButton::clicked, this, [this]() { emit needToDelete(); });
    funcLabelLay->addWidget(delBtn);

    // Ввод функции
    inputField = new QLineEdit(content);
    inputField->setPlaceholderText("Ввод Latex-функции");
    inputField->setStyleSheet("color: " + white + "; font: 18px; font-family: " + MathFont + "; border: 2px solid " +  light_lightgray + ";");
    int height = 35;
    //funcLabel->setFixedHeight(height);
    inputField->setFixedHeight(height);

    // Связываем с мат.операторами
    connect(inputField, &QLineEdit::cursorPositionChanged, this, [this] (int, int) {emit clicked(this);});

    // Связываем с изменением области выделения
    connect(inputField, &QLineEdit::selectionChanged, this, [this]() {
        QString sel = inputField->selectedText();
        // Если есть область выделения
        if (!sel.isEmpty()) {
            selectionStartPos = inputField->selectionStart();
            selectionLength = sel.length();
            selectionText = sel;
            select = true;
        }
        // Если пользователь сам ее сбросил
        else if (inputField->hasFocus()) {
            // выделение снято вручную, но поле в фокусе - сбрасываем
            select = false;
            selectionStartPos = -1;
            selectionLength = 0;
            selectionText.clear();
        }
    });
    // !
    QObject::connect(inputField, &QLineEdit::textChanged, funcLabel, [this](const QString& text) {
        funcLabel->setMath("$\\mathrm{\\color{" + white + "}{" + text + "}}$");
       // funcLabel->setMath("$\\mathrm{\\color{white} " + text + "}$");
        if (inputField->text() == "")
            funcLabel->setMath("$\\mathrm{\\color{" + white + "}{f(x)}}$");
    });

    // отрисовка функции
    connect(inputField, &QLineEdit::textChanged,
            this, &Functions::functionChanged);
    // !
    lay->addWidget(funcLabelContainer);
    lay->addWidget(inputField);

    QScrollArea* scroll = new QScrollArea;
    scroll->setWidgetResizable(true);
    scroll->setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    scroll->setHorizontalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    scroll->setWidget(content);

    QScrollBar* hScroll = scroll->horizontalScrollBar();
    hScroll->setStyleSheet(R"(
    QScrollBar:horizontal {
        border: none;
        background: )" + light_lightgray + R"(;
        height: 10px;
        border-radius: 5px;
    }
    QScrollBar::handle:horizontal {
        background: )" + blue + R"(;
        min-width: 20px;
        border-radius: 5px;
    }
    QScrollBar::handle:horizontal:hover {
        background: )" + blue_hover + R"(;
    }
    QScrollBar::add-line:horizontal, QScrollBar::sub-line:horizontal {
        height: 0px;
    }
    QScrollBar::add-page:horizontal, QScrollBar::sub-page:horizontal {
        background: none;
    }
)");

    // Главный контейнер класса
    Label_Input = new QWidget(parent);
    Label_Input->setMinimumHeight(120);
    QVBoxLayout* mainLayout = new QVBoxLayout(Label_Input);
    mainLayout->setContentsMargins(0,0,0,0);
    mainLayout->addWidget(scroll);
}
