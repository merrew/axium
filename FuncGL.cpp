#include "FuncGL.h"
#include <thread>

FuncGL::FuncGL(QWidget* parent, int ind, std::string func, double x_start, double x_end, double h, QPointF offset):
    id(ind),
    x_start(x_start), x_end(x_end),
    h(h),
    color(green),
    f(func),
    VAO(0), VBO(0)  {}

void FuncGL::init(QPointF offset)
{
    initializeOpenGLFunctions();

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    updateVertices(offset);

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V)*vertices.size(), vertices.data(), GL_STATIC_DRAW);

    // location = 0 -> aPos
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // location = 1 -> aColor
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}
void FuncGL::updateVertices(QPointF offset)
{
    vertices.clear();
    draw_segments.clear();

    int NUM_THREADS = std::thread::hardware_concurrency();
    std::vector<std::vector<V>> thread_vertices(NUM_THREADS);
    std::vector<std::vector<seg>> thread_segments(NUM_THREADS);

    float x0;
    float x1;
    if (f.type == FuncType::Forward)
    {
        x0 = x_start - offset.x();
        x1 = x_end + std::abs(offset.x());
    }
    else
    {
        x0 = x_start - offset.y();
        x1 = x_end + std::abs(offset.y());
    }

    const float h_min = 0.001f;
    const float h_max = 0.01f;
    const float slope_threshold = 0.1f;

    auto worker = [&](int tid) {
        std::vector<V>& local_vertices = thread_vertices[tid];
        std::vector<seg>& local_segments = thread_segments[tid];

        int offset_l = 0;
        int count = 0;
        float pred_y = 0.0f;
        bool first = true;

        float total_range = x1 - x0;
        float dx_thread = total_range / NUM_THREADS;
        float start_x = x0 + tid * dx_thread;
        float end_x   = start_x + dx_thread;

        float x = start_x;
        float h_current = h;

        while (x <= end_x)
        {
            std::optional<double> val_opt;
            if (f.type == FuncType::Forward)
                val_opt = f.eval({{"x", x}});
            else val_opt = f.eval({{"y", x}});

            if (!val_opt.has_value())
            {
                x += h_current;
                continue;
            }

            float y = val_opt.value();

            if (!first)
            {
                float slope = std::fabs((y - pred_y) / h_current);
                if (slope > slope_threshold) h_current = std::max(h_min, h_current * 0.25f);
                else if (slope < slope_threshold * 0.5f) h_current = std::min(h_max, h_current * 2.0f);

                if (std::fabs(y - pred_y) > 1e3)
                {
                    if (count > 0) local_segments.emplace_back(offset_l, count);
                    offset_l = local_vertices.size();
                    count = 0;
                }
            }

            if (f.type == FuncType::Forward)
                local_vertices.emplace_back(x, y, 0.0f, color.redF(), color.greenF(), color.blueF());
            else
                local_vertices.emplace_back(y, x, 0.0f, color.redF(), color.greenF(), color.blueF());
            pred_y = y;
            count++;
            first = false;

            x += h_current;
        }

        if (count > 0)
            local_segments.emplace_back(offset_l, count);
    };

    std::vector<std::thread> threads;
    for (int t = 0; t < NUM_THREADS; ++t)
        threads.emplace_back(worker, t);
    for (auto& th : threads)
        th.join();

    int vertex_offset = 0;
    for (int t = 0; t < NUM_THREADS; ++t)
    {
        for (auto& s : thread_segments[t])
            draw_segments.emplace_back(s.offset + vertex_offset, s.count);

        vertices.insert(vertices.end(), thread_vertices[t].begin(), thread_vertices[t].end());
        vertex_offset += thread_vertices[t].size();
    }

    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(V) * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void FuncGL::draw(QOpenGLShaderProgram& shader)
{
    glLineWidth(width);
    glBindVertexArray(VAO);


    for (auto& draw_segment : draw_segments)
    {
        // qDebug() << draw_segment.count << " " << draw_segment.offset << "\n";
        if (draw_segment.count < 2) continue;
        if (draw_segment.offset >= vertices.size()) draw_segment.offset = vertices.size() - 1;
        glDrawArrays(GL_LINE_STRIP, draw_segment.offset, draw_segment.count);
    }


    // glDrawArrays(GL_LINE_STRIP, 0, vertices.size());

    glBindVertexArray(0);
    glLineWidth(1.0f);

}

FuncGL::~FuncGL()
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
