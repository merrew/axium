#include "Fonts.h"

QString GlobalFont;
QString MathFont;
void loadFonts()
{
    // Глобальный шрифт
    int id1 = QFontDatabase::addApplicationFont(":/fonts/Ubuntu/Ubuntu-Light.ttf");
    GlobalFont = QFontDatabase::applicationFontFamilies(id1).front();
    QApplication::setFont(QFont(GlobalFont));

    // Математический шрифт
    int id2 = QFontDatabase::addApplicationFont(":/fonts/Math/STIXTwoMath-Regular.ttf");
    MathFont = QFontDatabase::applicationFontFamilies(id2).front();
}
