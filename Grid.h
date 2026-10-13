#ifndef GRID_H
#define GRID_H

#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QOpenGLWidget>
#include <QPainter>
#include <vector>

#include "Colors.h"
#include "Fonts.h"

class Grid : protected QOpenGLFunctions_3_3_Core
{
public:
    std::vector<float> vertices;

    double scale = 1.0;
    Grid(QWidget* parent = nullptr);

    void init(QPointF offset);
    void updateVertices(QPointF offset);
    void draw();

    int count_lines = 10;

    double x_start; double x_end;
    ~Grid();
private:
    GLuint VAO, VBO;

    int count_dotes;
};

#endif // GRID_H
