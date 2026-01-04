#include "OpenGLContainer.h"

OpenGLContainer::OpenGLContainer(Graph_function* graph, QWidget* parent)
    : QWidget(parent), graph_f(graph){}


void OpenGLContainer::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    int  size = qMax(width(), height());
    int x = (width() - size) / 2;
    int y = (height() - size) / 2;
    graph_f->setGeometry(x, y, size, size);

    QSize newSize = event->size();
    QSize oldSize = event->oldSize();
    double ratioW = (double)newSize.width() / oldSize.width();
    double ratioH = (double)newSize.height() / oldSize.height();
   // if (ratioW == 1.0 || ratioH == 1.0) return;
    double value = (ratioW + ratioH) / 2.0;
    graph_f->resizeFont(value);
}
