#pragma once

#include "toolspanel.h"

#include "QDebug"
struct LatexInfo
{
    QString latex_text;
    QString begin_symb;
    QString end_symb;
};

ToolsPanel::ToolsPanel(QMainWindow* window):  Panel("ФУНКЦИИ", window, Qt::LeftDockWidgetArea)
{
    // Списки операторов — каждая страница скролла
    QStringList page1 = {"a/b","xⁿ","ⁿ√x","|x|","logₙ","ln","lg","π","e"};
    QStringList page2 = {"sin","cos","tan","cot","arcsin","arccos","arctan","arccot"};
    QStringList page3 = {"sinh","cosh","tanh","coth","argsinh","argcosh","argtanh"};

    QList<QStringList> pages = {page1, page2, page3};

    stack_pages = new QStackedWidget(container);

    for (const QStringList &operators : pages)
    {
        Page* page = new Page(operators);

        page->setFocusPolicy(Qt::NoFocus);
        QObject::connect(page, &Page::operatorClicked, this, [this](const QString &op){

            // Вставка функции из мат.оператора
            const QMap<QString, LatexInfo> latexOps = {
                // page 1
                {"xⁿ",      {"", "", "^{}"}},
                {"ⁿ√x",     {"\\sqrt[]", "{", "}"}},
                {"|x|",     {"", "\\left| ( ", " ) \\right|"}},
                {"a/b",     {"\\frac", "{", "}{}"}},
                {"π",      {"\\pi", "", ""}},
                {"e",       {"e", "", ""}},
                {"ln",      {"\\ln", "(", ")"}},
                {"logₙ",     {"\\log_{}", "(", ")"}},
                {"lg",      {"\\log_{10}", "(", ")"}},
                // page 2
                {"sin",     {"\\sin", "(", ")"}},
                {"cos",     {"\\cos", "(", ")"}},
                {"tan",      {"\\tan", "(", ")"}},
                {"cot",     {"\\cot", "(", ")"}},
                {"arcsin",  {"\\arcsin", "(", ")"}},
                {"arccos",  {"\\arccos", "(", ")"}},
                {"arctan",   {"\\arctan", "(", ")"}},
                {"arccot",  {"\\textrm{arccot}", "(", ")"}},
                // page 3
                {"sinh",    {"\\sinh", "(", ")"}},
                {"cosh",    {"\\cosh", "(", ")"}},
                {"tanh",     {"\\tanh", "(", ")"}},
                {"coth",    {"\\coth", "(", ")"}},
                {"argsinh", {"\\textrm{argsinh}", "(", ")"}},
                {"argcosh", {"\\textrm{argcosh}", "(", ")"}},
                {"argtanh",  {"\\textrm{argtanh}", "(", ")"}}
            };
            if (!active_Func)
            {
                if (Func.empty()) return;
                active_Func = Func.front();
            }
            QString latex_text = active_Func->inputField->text();

            // Если есть выделенная область - обрамляем
            if (active_Func->select)
            {
                if (latexOps.contains(op))
                {
                    auto mathInfo = latexOps[op];
                    latex_text.replace(active_Func->selectionStartPos, active_Func->selectionLength, mathInfo.latex_text + mathInfo.begin_symb +
                                                                                                         active_Func->selectionText + mathInfo.end_symb);

                    active_Func->inputField->setText(latex_text);
                }
                active_Func->select = false;

            }
            // Иначе просто на место курсора вставляем
            else
            {
                if (latexOps.contains(op))
                {
                    int pos = active_Func->inputField->cursorPosition();
                    auto mathInfo = latexOps[op];

                    latex_text.insert(pos, mathInfo.latex_text + mathInfo.begin_symb + mathInfo.end_symb);
                    active_Func->inputField->setText(latex_text);
                }
            }

        });
        stack_pages->addWidget(page->page);
    }

    // Строка кружочков
    QWidget* tabsRow = new QWidget(container);
    QHBoxLayout* tabsLayout = new QHBoxLayout(tabsRow);
    tabsLayout->setContentsMargins(0,20,0,0);
    tabsLayout->setSpacing(1);


    // Создаём кнопки-кружки и привязываем их к вкладкам
    for (int i = 0; i < pages.size(); ++i)
    {
        CircleButton *c = new CircleButton(tabsRow);
        circleButtons.append(c);
        tabsLayout->addWidget(c);

        QObject::connect(c, &QPushButton::clicked, [this, i]() {
            // переключаемся на страницу i
            stack_pages->setCurrentIndex(i);
            // обновляем активность кружков
            for (int k=0;k<circleButtons.size();++k)
            {
                circleButtons[k]->SetActive(k==i);
            }
        });
    }
    if (!circleButtons.isEmpty()) circleButtons[0]->SetActive(true);

    // Кнопка создания новой функции
    addButtonContainer = new QWidget(container);
    QHBoxLayout* addButtonLayout = new QHBoxLayout(addButtonContainer);
    addButtonLayout->setContentsMargins(0, 20, 0, 20);
    QPushButton* addButton = new QPushButton("+", addButtonContainer);
    int size = 54;
    addButton->setFixedSize(size, size); // квадрат
    addButton->setStyleSheet(
        "QPushButton {"
        "  background-color: " + blue + ";"
                 "  color: white;"
                 "  font: 35px " + GlobalBoldFont + ";"
                           "  border-radius: 27px;"
                           "}"
                           "QPushButton:hover {"
                           "  background-color: " + blue_hover + ";"
                       "}"
        );
    connect(addButton, &QPushButton::clicked, this, &ToolsPanel::AddFunc);
    addButtonLayout->addWidget(addButton, Qt::AlignHCenter);

    vLayout->addWidget(tabsRow);
    vLayout->addWidget(stack_pages);

    // Ввод функций
    for (int i = 0; i < 3; i++)
    {
        AddFunc();
        //emit addFunc(Func.back()->inputField->text(), Func.back()->Color);
    }

    //vLayout->addStretch();
    vLayout->addWidget(addButtonContainer);

    setWidget(container);
}

void ToolsPanel::AddFunc()
{
    const int id = next_id++;

    Functions* f = new Functions(container, id);
    emit addFunc(f->inputField->text(), f->Color);
    qDebug() << "Tool::id = " << f->id;


    Func.emplace_back(f);

    int index = vLayout->indexOf(addButtonContainer);
    vLayout->insertWidget(index, f->Label_Input);

    // Связываем с математичекими операторами
    QObject::connect(f, &Functions::clicked, this, [this](Functions* self) {active_Func = self;});
    // Связываем с кнопкой удаления
    QObject::connect(f, &Functions::needToDelete, this, [this, f]() {
        // Удаляем виджет из layout
        vLayout->removeWidget(f->Label_Input);
        f->Label_Input->deleteLater(); // безопасно удаляем после события

        // Удаляем сам объект Functions
        auto it = std::find(Func.begin(), Func.end(), f);
        if (it != Func.end())
        {
            Func.erase(it);
            f->deleteLater(); // удаляем объект
        }
        vLayout->addStretch();
        active_Func = nullptr;
        emit removeFunc(f->id);
    });
    // Связывание с отрисовкой
    connect(f, &Functions::functionChanged,
            this, [this, f](const QString& t){
                qDebug() << "id = " << f->id;
                emit updateFunc(f->id, t);
            });

    connect(f, &Functions::colorFunctionChanged,
            this, [this, f](const QColor& c){
                qDebug() << "id = " << c;
                emit updateFuncColor(f->id, c);
            });

    //Func_size++;
}

