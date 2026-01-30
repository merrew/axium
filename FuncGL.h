#ifndef FUNCGL_H
#define FUNCGL_H
#include <vector>

#include <iostream>
#include <string>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QPainter>

#include "parser/FuncParse.h"
#include "Colors.h"

struct V {
    V() = default;
    float x, y, z;
    float r, g, b;
    V(float _x, float _y, float _z, float _r, float _g, float _b)
        : x(_x), y(_y), z(_z), r(_r), g(_g), b(_b) {}
};

struct seg { int offset; int count;
    seg(int _offset, int _count)
        : offset(_offset), count(_count) {}
};

// Экстремум
struct Extremum {
    double x; double y; bool is_max;
};
// Точки разрыва
enum class BreakPointType
{
    FirstType,
    SecondType
};
// Точки разрыва
struct BreakPoint
{
    double x;
    BreakPointType type;
};
// Промежутки монотонности
enum class IntervalMonoType
{
    Increasing,
    Decreasing
};
struct IntervalMono
{
    double start;
    double end;
    IntervalMonoType type;
};

class FuncGL : protected QOpenGLFunctions_3_3_Core
{
public:
    int id;
    float width = 2.5;
    double scale = 1.0;

    std::vector<seg> draw_segments;
    std::vector<V> vertices;
    double x_start; double x_end;
    double h;
    int nx;
    QColor color;
    std::string f_str;
    FuncParse f;
    void updateVertices(QPointF offset);

    explicit FuncGL(QWidget* parent = nullptr, int ind=0, std::string func="", double x_start=-10.0, double x_end=10.0, double h = 0.01, QPointF offset = {0.0, 0.0});

    //QVector<V> vertices;
    void init(QPointF offset);
    void draw(QOpenGLShaderProgram& shader);

    // Анализ
    bool eval(double x, double& y, double h=1e-3); // безопасное вычисление функции
    bool d_dx (double x, double& d, double h); // производная центральными разностями
    bool d2_dx2 (double x, double& d, double h); // 2 производная конечными разностями
    // Экстремумы
    std::vector<Extremum> findExtrema(
        double a,
        double b,
        std::function<bool(double, double&, double)> eval,
        std::function<bool(double, double&, double)> d_dx
        );
    // Пересечения с осями
    std::vector<QPointF> findNulls(
        double a,
        double b,
        std::function<bool(double, double&, double)> eval,
        std::function<bool(double, double&, double)> d_dx
        );
    // Точки разрыва
    std::vector<BreakPoint> findBreakPoints(
        double a,
        double b,
        std::function<bool(double, double&, double)> eval,
        std::function<bool(double, double&, double)> d_dx
        );
    // Промежутки монотонности
    std::vector<IntervalMono> findIntervalsMono(
        double a, double b,
        std::function<bool(double, double&, double)> eval,
        std::function<bool(double, double&, double)> d_dx);
    // Область определения
    std::vector<std::pair<double, double>> find_D(
        double a, double b,
        std::function<bool(double, double&, double)> eval,
        std::function<bool(double, double&, double)> d_dx);
    // Область значения
    std::vector<std::pair<double, double>> find_E(
        double a, double b,
        std::function<bool(double, double&, double)> eval,
        std::function<bool(double, double&, double)> d_dx);
    ~FuncGL();

private:

    GLuint VAO, VBO;


};



#endif // FUNCGL_H

