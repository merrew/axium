#ifndef OPENGLCONTAINER_H
#define OPENGLCONTAINER_H

#include <QWidget>
#include "Graph_function.h"

class OpenGLContainer : public QWidget
{
    Q_OBJECT
public:
    OpenGLContainer(Graph_function* graph, QWidget* parent=nullptr);
protected:
    void resizeEvent(QResizeEvent* event) override;
private:
    Graph_function* graph_f;
};

#endif // OPENGLCONTAINER_H
