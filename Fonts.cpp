#include "Fonts.h"

QString GlobalFont;
QString GlobalBoldFont;
QString MathFont;
void loadFonts()
{
    // Глобальный шрифт
    int id1 = QFontDatabase::addApplicationFont(":/fonts/Ubuntu/Ubuntu-Bold.ttf");
    GlobalFont = QFontDatabase::applicationFontFamilies(id1).front();
    QApplication::setFont(QFont(GlobalFont));

    // Глобальный жирный шрифт
    int id2 = QFontDatabase::addApplicationFont(":/fonts/Ubuntu/Ubuntu-Light.ttf");
    GlobalBoldFont = QFontDatabase::applicationFontFamilies(id2).front();

    // Математический шрифт
    int id3 = QFontDatabase::addApplicationFont(":/fonts/Math/STIXTwoMath-Regular.ttf");
    MathFont = QFontDatabase::applicationFontFamilies(id3).front();
}
