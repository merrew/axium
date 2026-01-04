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

    ~FuncGL();

private:

    GLuint VAO, VBO;


};



#endif // FUNCGL_H

