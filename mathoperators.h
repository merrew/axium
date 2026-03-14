#ifndef MATHOPERATORS_H
#define MATHOPERATORS_H
#include <QWidget>
#include <QPushButton>
#include <QPainter>
#include <QScrollArea>
#include <QStackedWidget>
#include <QVector>
#include <QObject>
#include <QGridLayout>

#include "Colors.h"
#include "Fonts.h"
// Переключатели вкладок (кружочки)
class CircleButton : public QPushButton
{
    Q_OBJECT
public:
    explicit CircleButton(QWidget* parent=nullptr);

    void SetActive(bool on);

    void paintEvent(QPaintEvent* event) override;
private:
    bool m_active;
};

// Математические операторы
class Page: public QWidget
{
    Q_OBJECT
public:
    QWidget *page;
    explicit Page(const QStringList &operators, QWidget* parent=nullptr);

signals:
    void operatorClicked(const QString &name);
};

#endif // MATHOPERATORS_H
