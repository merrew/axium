#ifndef GRAPH_FUNCTION_H
#define GRAPH_FUNCTION_H
#include <QOpenGLWidget>
#include <QOpenGLFunctions_3_3_Core>
#include <QOpenGLShaderProgram>
#include <QMouseEvent>
#include <QPainter>

#include "Axes.h"
#include "Grid.h"
#include "FuncGL.h"

#include "Colors.h"
#include "Fonts.h"
class Graph_function : public QOpenGLWidget, protected QOpenGLFunctions_3_3_Core
{
    Q_OBJECT
public:
    int next_id = 0;
    explicit Graph_function(QWidget* parent = nullptr);
    void resizeFont(double value);
    void setDefaultFont();

protected:
    void initializeGL() override;
    void resizeGL(int w, int h) override;
    void paintGL() override;




    ~Graph_function();
private:
    bool showPoint = false;
    QPointF pointScreen;
    double pointWorld_x;
    double pointWorld_y;
    QColor pointColor;
    QOpenGLShaderProgram shader;
    // GLuint VAO, VBO;

    Axes axes;
    Grid grid;
    std::vector<std::unique_ptr<FuncGL>> funcs;
    bool findPoint(const QPointF& mousePos, QPointF& screenPos, double& WorldX, double& WorldY);
    void mousePressEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;

    QPointF lastMousePos;
    QPointF offset = {0.0, 0.0};
    double scale = 1.0;
    double scale_max = 50.0;
    double scaleFont = 1.0;

signals:
    void rightClicked();
public slots:
              // void setFunction(const QString& funcText);
              // void setColorFunction(const QColor& color);
    void addFunc(const QString& expr, const QColor& color);
    void updateFunc(int index, const QString& expr);
    void updateFuncColor(int index, const QColor& color);
    void removeFunc(int index);
};

#endif // GRAPH_FUNCTION_H
