#include "Graph_function.h"

#include <QOpenGLWidget>
Graph_function::Graph_function(QWidget* parent)
    : QOpenGLWidget(parent) {}

void Graph_function::addFunc(const QString& expr, const QColor& color)
{
    makeCurrent();

    auto f = std::make_unique<FuncGL>(this, next_id++, expr.toStdString(), -scale, scale, 2 * scale/width(), offset);

    qDebug() << "func.f_str = " << f->f_str;
    f->f.printLex();
    f->f.printAST();
    f->h = (f->x_end - f->x_start) / width();
    f->color = color;
    f->init(offset);

    funcs.push_back(std::move(f));
    update();
}

void Graph_function::updateFunc(int index, const QString& expr)
{
    auto it = std::find_if(funcs.begin(), funcs.end(),
                           [index](const std::unique_ptr<FuncGL>& f) {
                               return f->id == index;
                           });

    (*it)->f_str = expr.toStdString();
    (*it)->f.setExpr((*it)->f_str);
    (*it)->f.printLex();
    (*it)->f.printAST();
    (*it)->updateVertices(offset);
    update();
}

void Graph_function::updateFuncColor(int index, const QColor& color)
{
    auto it = std::find_if(funcs.begin(), funcs.end(),
                           [index](const std::unique_ptr<FuncGL>& f) {
                               return f->id == index;
                           });
    (*it)->color = color;
    (*it)->updateVertices(offset);
    update();
}

void Graph_function::removeFunc(int index)
{
    makeCurrent();

    auto it = std::find_if(funcs.begin(), funcs.end(),
                           [index](const std::unique_ptr<FuncGL>& f) {
                               return f->id == index;
                           });

    if (it != funcs.end())
        funcs.erase(it);
    // next_id--;
    update();
}

void Graph_function::initializeGL()
{
    initializeOpenGLFunctions();

    scale = 1.0 * grid.count_lines;

    // Инициализация
    grid.scale = scale;
    grid.init(offset);

    axes.scale = scale;
    axes.init(offset);

    for (int i = 0; i < 3; i++)
    {
        addFunc("", green);
    }

    //func.init();


    QColor background_c = QColor(gray);

    glClearColor(background_c.redF(), background_c.greenF(), background_c.blueF(), 1.0f);

    shader.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/vertex.vert");
    shader.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/fragment.frag");

    shader.link();
}

void Graph_function::resizeGL(int w, int h)
{
    glViewport(0, 0, w, h);
}

void Graph_function::paintGL()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // Отрисовка треугольника
    shader.bind();

    // Матрица вида (камера)
    QMatrix4x4 view;
    view.setToIdentity();
    view.translate(offset.x(), offset.y(), 0.0f);  // "отодвигаем" камеру назад, иначе не увидим объект

    // Матрица проекции (перспективная)
    QMatrix4x4 projection;
    projection.setToIdentity();
    projection.ortho(
        -scale, scale,
        -scale, scale,
        -1, 1
        );
    QMatrix4x4 model;
    model.setToIdentity();

    // Передаём матрицы в шейдер
    shader.setUniformValue("view", view);
    shader.setUniformValue("model", model);
    shader.setUniformValue("projection", projection);


    // Оси координат
    grid.draw();
    axes.draw();
    for (auto& func : funcs)
    {
        func->draw();
    }

    shader.release();

    // Отрисовка подписей координат
    QPainter painter(this);
    painter.setRenderHint(QPainter::TextAntialiasing);
    painter.setPen(white);
    int fontSize = qMin(12.0, 12 * scaleFont);
    painter.setFont(QFont(MathFont, fontSize));

    float step = (2.0f * scale) / grid.count_lines;

    float left   = -offset.x() - scale * 1.0f;
    float right  = -offset.x() + scale * 1.0f;
    float bottom = -offset.y() - scale * 1.0f;
    float top    = -offset.y() + scale * 1.0f;

    float x_min = std::floor(left / step) * step;
    float x_max   = std::ceil(right / step) * step;
    float y_min = std::floor(bottom / step) * step;
    float y_max   = std::ceil(top / step) * step;

    double x_step = step;
    double y_step = step;

    // X-оси
    for (double x = x_min; x <= x_max; x += x_step)
    {
        if (std::abs(x) < 1e-6) continue;

        double screenX = (0.5 + (x + offset.x()) / (2 * scale)) * width();
        double screenY = (0.5 - (0.0 + offset.y()) / (2 * scale)) * height();

        QString label = QString::number(x, 'f', 2);
        painter.drawText(QPointF(screenX - 10, screenY + 15), label);
    }

    // Y-оси
    for (double y = y_min; y <= y_max; y += y_step)
    {
        if (std::abs(y) < 1e-6) continue;

        double screenX = (0.5 + (0.0 + offset.x()) / (2 * scale)) * width();
        double screenY = (0.5 - (y + offset.y()) / (2 * scale)) * height();

        QString label = QString::number(y, 'f', 3);
        painter.drawText(QPointF(screenX + 5, screenY), label);
    }

    if (showPoint)
    {
        painter.setPen(Qt::NoPen);
        painter.setBrush(pointColor);
        double radius = 5.0;
        painter.drawEllipse(screenPos, radius, radius);

        painter.setPen(white);
        int fontSize = qMin(12.0, 12 * scaleFont);
        painter.setFont(QFont(MathFont, fontSize));
        QString label = "(" + QString::number(pointWorld_x, 'f', 3) + " ; " + QString::number(pointWorld_y, 'f', 3) + ")";
        painter.drawText(screenPos + QPointF(radius+2, -radius-2), label);
    }
    painter.end();

}

void Graph_function::resizeFont(double value)
{
    if (std::isnan(value) || value <= 0.0 || value > 10.0)
        return;

    scaleFont *= value;
    update();
}

void Graph_function::setDefaultFont()
{
    scaleFont = 1.0;
}

bool Graph_function::findPoint(const QPointF& mousePos, QPointF& screenPos, double& WorldX, double& WorldY)
{
    WorldX =
        (mousePos.x() / width() - 0.5) * 2.0 * scale - offset.x();
    float cursorWorldY =
        (0.5 - mousePos.y() / height()) * 2.0 * scale - offset.y();

    float minDist = 0.3;
    bool find = false;
    for (auto& func : funcs)
    {
        func->width = 2.5;
        double y, dy;
        if (func->f.type == FuncType::Forward)
        {
            y = func->f.eval({{"x", WorldX}}).value();
            dy = std::abs(y - cursorWorldY);
        }
        else
        {
            y = func->f.eval({{"y", cursorWorldY}}).value();
            dy = std::abs(y - WorldX);
        }
        if (dy < minDist)
        {
            WorldY = y;
            if (func->f.type == FuncType::Forward)
            {
                screenPos.setX((0.5 + (WorldX + offset.x()) / (2*scale)) * width());
                screenPos.setY((0.5 - (y + offset.y()) / (2*scale)) * height());
            }
            else
            {
                screenPos.setX((0.5 + (y + offset.x()) / (2*scale)) * width());
                screenPos.setY((0.5 - (cursorWorldY + offset.y()) / (2*scale)) * height());
            }
            pointColor = func->color;
            func->width = 5;
            emit functionSelected(func.get());
            find = true;
        }
    }
    return find;
}
void Graph_function::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton)
    {
        showPoint = findPoint(event->position(), screenPos, pointWorld_x, pointWorld_y);
        update();
    }
    if (event->button() == Qt::RightButton)
        emit rightClicked();

    if (event->button() == Qt::MiddleButton)
        lastMousePos = event->pos();
}

void Graph_function::mouseMoveEvent(QMouseEvent* event)
{
    if (event->buttons() & Qt::LeftButton)
    {
        showPoint = findPoint(event->position(), screenPos, pointWorld_x, pointWorld_y);
        update();
    }
    if (event->buttons() & Qt::MiddleButton)
    {
        QPointF delta = event->pos() - lastMousePos;
        offset.rx() += 2.0 * delta.x() / width() * scale;
        offset.ry() -= 2.0 * delta.y() / height() * scale;
        lastMousePos = event->pos();

        grid.updateVertices(offset);
        axes.updateVertices(offset);

        for (auto& func : funcs)
        {
            func->h = (func->x_end - func->x_start) / width();
            func->updateVertices(offset);
        }
        update();
    }
}

void Graph_function::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::LeftButton)
    {
        showPoint = false;
        update();
    }
}

void Graph_function::wheelEvent(QWheelEvent* event)
{

    QPointF mousePos = event->position();

    double worldX =
        (mousePos.x() / width() - 0.5) * 2.0 * scale - offset.x();
    double worldY =
        (0.5 - mousePos.y() / height()) * 2.0 * scale - offset.y();

    double factor = event->angleDelta().y() > 0 ? 1.1 : 0.9;
    double newScale = scale / factor;
    newScale = std::clamp(newScale, 0.05, scale_max);

    offset.rx() =
        (mousePos.x() / width() - 0.5) * 2.0 * newScale - worldX;

    offset.ry() =
        (0.5 - mousePos.y() / height()) * 2.0 * newScale - worldY;

    scale = newScale;

    for (auto& func : funcs)
    {
        func->x_start = -scale;
        func->x_end   =  scale;
        func->scale = scale;
        func->h = (func->x_end - func->x_start) / width();
        func->updateVertices(offset);
    }

    axes.scale = scale;
    axes.updateVertices(offset);

    grid.scale = scale;
    grid.updateVertices(offset);

    update();
}


Graph_function::~Graph_function()
{
    /*
    if (QOpenGLContext::currentContext() != nullptr)
    {
        if (VAO != 0)
        {
            glDeleteVertexArrays(1, &VAO);
            VAO = 0;
        }
        if (VBO != 0)
        {
            glDeleteBuffers(1, &VBO);
            VBO = 0;
        }
    }
*/
    funcs.clear();
}

