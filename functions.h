#ifndef FUNCTIONS_H
#define FUNCTIONS_H
#include <QObject>
#include <QScrollArea>
#include <QScrollBar>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QColorDialog>
#include <QPushButton>
#include <QWidget>

#include "Colors.h"
#include "Fonts.h"

#include "jkqtmathtext/jkqtmathtextlabel.h"
#include "jkqtmathtext/jkqtmathtext.h"

class Functions: public QWidget
{
    Q_OBJECT
public:
    QColor Color = green;
    QWidget* Label_Input;
    JKQTMathTextLabel* funcLabel;
    QLineEdit* inputField;

    bool select = false;
    int selectionStartPos;
    int selectionLength;
    QString selectionText;
    int id;
    explicit Functions(QWidget* parent=nullptr, int ind=0);

signals:
   void needToDelete();
   void clicked(Functions* self);
   void functionChanged(const QString& newText);
   void colorFunctionChanged(QColor newColor);
};


#endif // FUNCTIONS_H
