#include "Axes.h"

Axes::Axes(QWidget* parent) : VAO(0), VBO(0) {}

void Axes::init(QPointF offset)
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    updateVertices(offset);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // location = 0 -> aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // location = 1 -> aColor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void Axes::updateVertices(QPointF offset)
{
    float left   = -scale - (offset.x());
    float right  =  scale - (offset.x());
    float bottom = -scale - (offset.y());
    float top    =  scale - (offset.y());

    // Оси
    QColor c = QColor(white);

    vertices.clear();

    vertices.insert(vertices.end(), {
        left, 0.0f, 0.0f, c.redF(), c.greenF(), c.blueF(),
        right, 0.0f,    0.0f, c.redF(), c.greenF(), c.blueF()
    });
    vertices.insert(vertices.end(), {
        0.0f, bottom, 0.0f, c.redF(), c.greenF(), c.blueF(),
        0.0f, top,    0.0f, c.redF(), c.greenF(), c.blueF()
    });

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Axes::draw()
{
    //shader.setUniformValue("model", model);
    glLineWidth(3.0f);
    glBindVertexArray(VAO);
    glDrawArrays(GL_LINES, 0, 4);
    glBindVertexArray(0);
    glLineWidth(1.0f);

}

Axes::~Axes()
{
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

