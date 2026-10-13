#ifndef AXES_H
#define AXES_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLWidget>
#include <QOpenGLShaderProgram>
#include <QPainter>
#include <vector>

#include "Colors.h"
#include "Fonts.h"

class Axes : protected QOpenGLFunctions_3_3_Core
{
public:
    std::vector<float> vertices;
    double scale = 1.0;
    Axes(QWidget* parent=nullptr);

    void init(QPointF offset);
    void updateVertices(QPointF offset);
    void draw();

    ~Axes();
private:
    GLuint VAO, VBO;
};

#endif // AXES_H
