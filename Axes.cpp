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
    float left   = -scale - std::abs(offset.x());
    float right  =  scale + std::abs(offset.x());
    float bottom = -scale - std::abs(offset.y());
    float top    =  scale + std::abs(offset.y());

    // Оси
    QColor c = QColor(white);

    vertices.clear();

    vertices.push_back(left);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(c.redF());
    vertices.push_back(c.greenF());
    vertices.push_back(c.blueF());

    vertices.push_back(right);
    vertices.push_back(0.0f);
    vertices.push_back(0.0f);
    vertices.push_back(c.redF());
    vertices.push_back(c.greenF());
    vertices.push_back(c.blueF());

    vertices.push_back(0.0f);
    vertices.push_back(bottom);
    vertices.push_back(0.0f);
    vertices.push_back(c.redF());
    vertices.push_back(c.greenF());
    vertices.push_back(c.blueF());

    vertices.push_back(0.0f);
    vertices.push_back(top);
    vertices.push_back(0.0f);
    vertices.push_back(c.redF());
    vertices.push_back(c.greenF());
    vertices.push_back(c.blueF());

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void Axes::draw(QOpenGLShaderProgram& shader, const QMatrix4x4& model)
{
    shader.setUniformValue("model", model);
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

