#include "Grid.h"

Grid::Grid(QWidget* parent) : VAO(0), VBO(0) {}

void Grid::init(QPointF offset)
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    updateVertices(offset);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

    // location = 0 -> aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // location = 1 -> aColor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Grid::updateVertices(QPointF offset)
{
    vertices.clear();

    QColor c = QColor(white_gray);
    float step = (2.0f * scale) / count_lines;

    // Вычисляем видимые границы с запасом на движение
    float left   = -offset.x() - scale * 2.0f;  // запас = 2*scale
    float right  = -offset.x() + scale * 2.0f;
    float bottom = -offset.y() - scale * 2.0f;
    float top    = -offset.y() + scale * 2.0f;

    // Смещаем линии к ближайшему кратному step
    float x_start = std::floor(left / step) * step;
    float x_end   = std::ceil(right / step) * step;

    for (float x = x_start; x <= x_end; x += step)
    {
        vertices.insert(vertices.end(), {
                                            x, bottom, 0.0f, c.redF(), c.greenF(), c.blueF(),
                                            x, top,    0.0f, c.redF(), c.greenF(), c.blueF()
                                        });
    }

    float y_start = std::floor(bottom / step) * step;
    float y_end   = std::ceil(top / step) * step;

    for (float y = y_start; y <= y_end; y += step)
    {
        vertices.insert(vertices.end(), {
                                            left,  y, 0.0f, c.redF(), c.greenF(), c.blueF(),
                                            right, y, 0.0f, c.redF(), c.greenF(), c.blueF()
                                        });
    }


    count_dotes = vertices.size() / 6;

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER,
                 vertices.size() * sizeof(float),
                 vertices.data(),
                 GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void Grid::draw(QOpenGLShaderProgram& shader, const QMatrix4x4& model)
{
/*
    shader.bind();

    QMatrix4x4 ortho;
    ortho.setToIdentity();
    ortho.ortho(-1, 1, -1, 1, -1, 1);

    shader.setUniformValue("model", QMatrix4x4());
    shader.setUniformValue("view", QMatrix4x4());
    shader.setUniformValue("projection", ortho);
*/

    shader.setUniformValue("model", model);

    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, count_dotes);
    glBindVertexArray(0);

}

Grid::~Grid()
{
    // Убедимся, что контекст активен перед удалением OpenGL-объектов
    if (QOpenGLContext::currentContext() != nullptr) {
        if (VAO != 0) {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0) {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
    }
}

