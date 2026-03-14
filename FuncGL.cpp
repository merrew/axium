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

    /*
    if (f.correct)
    {
        auto ex = findExtrema(x0, x1,
          [this](double x, double& y, double h) {
            return this->eval(x, y, h);
        },
          [this](double x, double& d, double h) {
              return this->d_dx(x, d, h);
          });
        auto nl = findNulls(x0, x1,
            [this](double x, double& y, double h) {
                return this->eval(x, y, h);
            },
            [this](double x, double& d, double h) {
                return this->d_dx(x, d, h);
            });
        auto bp = findBreakPoints(x0, x1,
              [this](double x, double& y, double h) {
                  return this->eval(x, y, h);
              },
              [this](double x, double& d, double h) {
                  return this->d_dx(x, d, h);
              });
        auto in_mon = findIntervalsMono(x0, x1,
                [this](double x, double& y, double h) {
                    return this->eval(x, y, h);
                },
                [this](double x, double& d, double h) {
                    return this->d_dx(x, d, h);
                });
        auto D = find_D(x0, x1,
                [this](double x, double& y, double h) {
                    return this->eval(x, y, h);
                },
                [this](double x, double& d, double h) {
                    return this->d_dx(x, d, h);
                });
        auto E = find_E(x0, x1,
                [this](double x, double& y, double h) {
                    return this->eval(x, y, h);
                },
                [this](double x, double& d, double h) {
                    return this->d_dx(x, d, h);
                });
        auto ex2 = findExtrema(x0, x1,
              [this](double x, double& y, double h) {
                  return this->d_dx(x, y, h);
              },
              [this](double x, double& d, double h) {
                  return this->d2_dx2(x, d, h);
              });
        auto bp2 = findBreakPoints(x0, x1,
              [this](double x, double& y, double h) {
                  return this->d_dx(x, y, h);
              },
              [this](double x, double& d, double h) {
                  return this->d2_dx2(x, d, h);
              });
        auto crit2_intervals = findIntervalsMono(x0, x1,
             [this](double x, double& y, double h) {
                 return this->d_dx(x, y, h);
             },
             [this](double x, double& d, double h) {
                 return this->d2_dx2(x, d, h);
             });


        for (auto& e : ex)
                qDebug() << (e.is_max ? "Максимум: " : "Минимум: ") << "(" << e.x << ";" << e.y << ")\n";
        for (auto& e : nl)
            qDebug() << "Пересечение с осью: " << "(" << e.rx() << ";" << e.ry() << ")\n";
        for (auto& e : bp)
            qDebug() << (e.type == BreakPointType::FirstType ? "Разрыв 1 рода: x = " : "Разрыв 2 рода: x = ") << "(" << e.x << ")\n";
        for (auto& e : in_mon)
            qDebug() << (e.type == IntervalMonoType::Increasing ? "Возрастание: (" : "Убывание: (") << e.start << "; " << e.end << ")\n";
        qDebug() << "Область определения: ";
        for (auto& e : D)
            qDebug() << "(" << e.first << ";" << e.second << ") U ";
        qDebug() << "Область значения: ";
        for (auto& e : E)
            qDebug() << "(" << e.first << ";" << e.second << ") U ";
        for (auto& e : ex2)
            qDebug() << (e.is_max ? "Восходящая точка перегиба: " : "Нисходящая точка перегиба: ") << "(" << e.x << ";" << e.y << ")\n";
        for (auto& e : crit2_intervals)
            qDebug() << (e.type == IntervalMonoType::Increasing ? "Вогнутость: (" : "Выпуклость: (") << e.start << "; " << e.end << ")\n";

        qDebug() << "Интеграл на [" << x0 << ";" << x1 << "]" << "методом прямоугольников = " << rectangle_integrate(x0, x1);
        qDebug() << "Интеграл на [" << x0 << ";" << x1 << "]" << "методом трапеций = " << trapezoid_integrate(x0, x1);
        qDebug() << "Интеграл на [" << x0 << ";" << x1 << "]" << "методом Симпсона = " << simpson_integrate(x0, x1);
        qDebug() << "Интеграл на [" << x0 << ";" << x1 << "]" << "методом Гаусса = " << gauss_composite_integrate(x0, x1);
        qDebug() << "Длина дуги на [" << x0 << ";" << x1 << "]" << " = " << curve_length(x0, x1);

        qDebug() << "\n";

    }

*/
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

bool FuncGL::eval(double x, double& y, double h)
{
    auto value = (f.type == FuncType::Forward)
    ? f.eval({{"x", x}})
    : f.eval({{"y", x}});
    if (!value.has_value()) return false;
    y = value.value();
    return !std::isinf(y);
};

bool FuncGL::d_dx (double x, double& d, double h)
{
    double y1, y2;
    if (!eval(x + h, y1) || !eval(x - h, y2))
        return false;

    d = (y1 - y2) / 2*h;
    return !std::isinf(d);
};

bool FuncGL::d2_dx2 (double x, double& d, double h)
{
    double y0, y1, y2;
    if (!eval(x - h, y0) || !eval(x, y1) || !eval(x + h, y2))
        return false;

    d = (y0 - 2*y1 + y2) / h*h;
    return !std::isinf(d);
};

std::vector<Extremum> FuncGL::findExtrema(
    double a,
    double b,
    std::function<bool(double, double&, double)> eval,
    std::function<bool(double, double&, double)> d_dx
    )
{
    const float h_min = (b - a) / 10000;
    const float h_max = (b - a) / 100;
    double h_l = h_max;

    std::vector<Extremum> extremum;

    double x = a;

    auto secant_method = [&](double x0, double x1)
    {
        double d0, d1;
        if (!d_dx(x0, d0, h_l) || !d_dx(x1, d1, h_l)) return x1;

        for (int i = 0; i < 100; i++)
        {
            if (fabs(d1 - d0) < 1e-8) break;

            double x2 = x1 - d1 * (x1 - x0) / (d1 - d0);
            if (std::fabs(x2 - x1) < h_l / 10)
                break;

            x0 = x1;
            d0 = d1;
            x1 = x2;
        }
        return x1;
    };


    while (x + h_l <= b)
    {
        double y0, y1, y2;

        if (!eval(x - h_l, y0, h) || !eval(x, y1, h) || !eval(x + h_l, y2, h))
        {
            x += h_l;
            continue;
        }

        // максимум
        if (y0 < y1 && y1 > y2)
        {
            // уточнение параболой
            double xe = x + h_l * (y0 - y2) / (2*(y0 - 2*y1 + y2));

            // уточнение методом секущих
            xe =  secant_method(x - h_l, x + h_l);

            auto break_points = findBreakPoints(xe-1e-2, xe+1e-2, eval, d_dx);
            if (break_points.empty())
            {
                double ye;
                if (eval(xe, ye, h))
                {
                    if (f.type == FuncType::Forward)
                        extremum.push_back({xe, ye, true});
                    else
                        extremum.push_back({ye, xe, true});
                }
            }
        }

        // минимум
        if (y0 > y1 && y1 < y2)
        {
            // уточнение параболой
            double xe = x + h_l * (y0 - y2) / (2*(y0 - 2*y1 + y2));

            // уточнение методом секущих
            xe =  secant_method(x - h_l, x + h_l);

            auto break_points = findBreakPoints(xe-1e-2, xe+1e-2, eval, d_dx);
            if (break_points.empty())
            {
                double ye;

                if (eval(xe, ye, h))
                {
                    if (f.type == FuncType::Forward)
                        extremum.push_back({xe, ye, false});
                    else
                        extremum.push_back({ye, xe, false});
                }
            }
        }

        double slope = fabs((y2 - y0) / (2*h_l));
        if (slope > 1.0)
            h_l = std::fmax(h_min, h_l * 0.5);
        else if (slope < 0.2)
            h_l = std::fmin(h_max, h_l * 2.0);

        x += h_l;
    }

    return extremum;
}

std::vector<QPointF> FuncGL::findNulls(
    double a,
    double b,
    std::function<bool(double, double&, double)> eval,
    std::function<bool(double, double&, double)> d_dx
    )
{
    const float h_min = (b - a) / 10000;
    const float h_max = (b - a) / 100;
    double h_l = h_max;

    std::vector<QPointF> nulls;

    auto secant_method = [&](double x0, double x1)
    {
        double y0, y1;
        if (eval(x0, y0, h) || !eval(x1, y1, h)) return x1;

        for (int i = 0; i < 100; i++)
        {
            if (fabs(y1 - y0) < 1e-8) break;

            double x2 = x1 - y1 * (x1 - x0) / (y1 - y0);
            if (std::fabs(x2 - x1) < h_l / 10)
                break;

            x0 = x1;
            y0 = y1;
            x1 = x2;
        }
        return x1;
    };

    double x = a;
    double y_prev;
    while (!eval(x, y_prev, h))
    {
        x += h_l;
        continue;
    }

    while (x + h_l <= b)
    {
        double x_next = x + h_l;
        double y_next;

        if (!eval(x_next, y_next, h))
        {
            x = x_next;
            continue;
        }

        // проверка смены знака
        if (y_prev * y_next <= 0)
        {
            double root = secant_method(x, x_next);
            auto break_points = findBreakPoints(root-1e-2, root+1e-2, eval, d_dx);
            if (break_points.empty())
            {
                double yroot;
                if (eval(root, yroot, h))
                {
                    if (f.type == FuncType::Forward)
                        nulls.push_back({root, 0});
                    else
                        nulls.push_back({0, root});
                }
            }
        }

        double slope = fabs((y_next - y_prev) / h_l);
        if (slope > 1.0)
            h_l = std::fmax(h_min, h_l * 0.5);
        else if (slope < 0.2)
            h_l = std::fmin(h_max, h_l * 2.0);

        x = x_next;
        y_prev = y_next;
    }

    return nulls;
}

std::vector<BreakPoint> FuncGL::findBreakPoints(
    double a,
    double b,
    std::function<bool(double, double&, double)> eval,
    std::function<bool(double, double&, double)> d_dx
    )
{
    std::vector<BreakPoint> breakpoints;

    double h_l = 1e-4;
    for (double x = a; x <= b; x += h_l)
    {

        double y0, y2;
        double eps = 1e4;
        // точка разрыва 2 рода
        auto find = std::find_if(breakpoints.begin(), breakpoints.end(), [&](BreakPoint el){return fabs(x - el.x) <= 0.5;});
        if (find != breakpoints.end()) continue;
        if (!eval(x - h_l, y0, h) || !eval(x + h_l, y2, h))
        {
            breakpoints.push_back({x, BreakPointType::SecondType});
            continue;
        } else if (fabs(y2 - y0) > eps)
        {
            // уточнение
            double y0_2, y2_2;
            eval(x - h_l/2, y0_2, h);
            eval(x + h_l/2, y2_2, h);

            if (fabs(y0_2 - y0) > eps || fabs(y2_2 - y2) > eps)
            {
                breakpoints.push_back({x, BreakPointType::SecondType});
                continue;
            }
            else
            {
                breakpoints.push_back({x, BreakPointType::FirstType});
                continue;
            }
        }
        else if (std::isnan(y0) && !isnan(y2))
            breakpoints.push_back({x, BreakPointType::SecondType});
    }

    return breakpoints;
}

std::vector<IntervalMono> FuncGL::findIntervalsMono(
    double a, double b,
    std::function<bool(double, double&, double)> eval,
    std::function<bool(double, double&, double)> d_dx)
{
    std::vector<IntervalMono> intervals_mono;

    auto breakpoints = findBreakPoints(a, b, eval, d_dx);
    auto extremum = findExtrema(a, b, eval, d_dx);

    std::vector<double> critical;
    critical.push_back(a);
    critical.push_back(b);

    for (auto& bp : breakpoints)
        critical.push_back(bp.x);

    for (auto& e : extremum)
        critical.push_back(e.x);

    std::sort(critical.begin(), critical.end());

    critical.erase(
        std::unique(critical.begin(), critical.end(),
                    [&](double x1, double x2)
                    {
                        return std::fabs(x1 - x2) < (b - a) / 1000;
                    }),
        critical.end()
        );

    for (int i = 0; i + 1 < critical.size(); ++i)
    {
        double x0 = critical[i];
        double x1 = critical[i + 1];

        double d;
        if (!d_dx(0.5 * (x0 + x1), d, 1e-3))
            continue;

        if (d > 0)
            intervals_mono.push_back({x0, x1, IntervalMonoType::Increasing});
        else if (d < 0)
            intervals_mono.push_back({x0, x1, IntervalMonoType::Decreasing});
    }
    return intervals_mono;
}


std::vector<std::pair<double, double>> FuncGL::find_D(
    double a, double b,
    std::function<bool(double, double&, double)> eval,
    std::function<bool(double, double&, double)> d_dx)
{
    std::vector<std::pair<double, double>> D;

    auto breakpoints = findBreakPoints(a, b, eval, d_dx);
    breakpoints.insert(breakpoints.begin(), {a, BreakPointType::FirstType});
    breakpoints.push_back({b, BreakPointType::FirstType});

    auto it = breakpoints.begin();
    auto next = std::next(it);

    while (next != breakpoints.end())
    {
        double x0 = it->x;
        double x1 = next->x;

        double y_h2;
        if (eval(x0 + (x1 - x0), y_h2, h) && !std::isnan(y_h2))
            D.emplace_back(x0, x1);

        ++it;
        ++next;
    }
    return D;
}

std::vector<std::pair<double, double>> FuncGL::find_E(
    double a, double b,
    std::function<bool(double, double&, double)> eval,
    std::function<bool(double, double&, double)> d_dx)
{
    std::vector<std::pair<double, double>> E;
    auto extremum = findExtrema(a, b, eval, d_dx);

    auto breakpoints = findBreakPoints(a, b, eval, d_dx);
    breakpoints.insert(breakpoints.begin(), {a, BreakPointType::FirstType});
    breakpoints.push_back({b, BreakPointType::FirstType});

    std::vector<std::pair<double, double>> y_ranges;

    auto it = breakpoints.begin();
    auto next = std::next(it);

    while (next != breakpoints.end())
    {
        double x0 = it->x;
        double x1 = next->x;


        double y_h2;
        if (!eval(x0 + (x1 - x0), y_h2, h) || std::isnan(y_h2))
        {
            ++it;
            ++next;
            continue;
        }

        std::vector<double> y;
        for (auto ex : extremum)
        {
            if (ex.x > x0 && ex.x < x1)
            {
                y.emplace_back(ex.y);
            }
        }
        double y_x0, y_x1;
        double y_plus_h, y_plus_h1;
        double y_minus_h, y_minus_h1;


        eval(x0, y_x0, h); eval(x1, y_x1, h);
        eval(x0 + 1e-3, y_plus_h, h); eval(x0 + 1e-3 + 1e-2, y_plus_h1, h);
        eval(x1 - 1e-3, y_minus_h, h); eval(x1 - 1e-3 - 1e-2, y_minus_h1, h);

        while(isnan(y_x0) || isnan(y_plus_h) || isnan(y_plus_h1))
        {
            x0 += 1e-3;
            eval(x0, y_x0, h);
            eval(x0 + 1e-3, y_plus_h, h);
            eval(x0 + 1e-3 + 1e-2, y_plus_h1, h);
        }
        while(isnan(y_x1) || isnan(y_minus_h) || isnan(y_minus_h1))
        {
            x1 -= 1e-3;
            eval(x1, y_x1, h);
            eval(x1 - 1e-3, y_minus_h, h);
            eval(x1 - 1e-3 - 1e-2, y_minus_h1, h);
        }
        if (it->type == BreakPointType::SecondType && std::fabs(y_plus_h - y_plus_h1) > (x0 + 1e-3 - (x0 + 1e-3 + 1e-2)))
        {
            if (y_plus_h > 0)
                y.emplace_back(std::numeric_limits<double>::infinity());
            else if (y_plus_h < 0)
                y.emplace_back(-std::numeric_limits<double>::infinity());
            y.emplace_back(y_x1);
        }
        else if (next->type == BreakPointType::SecondType && std::fabs(y_minus_h - y_minus_h1) > (x1 - 1e-3 - (x1 - 1e-3 - 1e-2)))
        {

            if (y_minus_h > 0)
                y.emplace_back(std::numeric_limits<double>::infinity());
            else if (y_minus_h < 0)
                y.emplace_back(-std::numeric_limits<double>::infinity());
            y.emplace_back(y_x0);
        }
        else
        {
            y.emplace_back(y_x0);
            y.emplace_back(y_x1);
        }


        y_ranges.push_back(
            {*(std::min_element(y.begin(), y.end())),
             *(std::max_element(y.begin(), y.end()))}
            );

        ++it;
        ++next;
    }

    std::sort(y_ranges.begin(), y_ranges.end(),
              [](auto& a, auto& b)
              {
                  return a.first < b.first;
              });
    for(auto& el : y_ranges)
        qDebug() << "Промежутки: (" << el.first << ";" << el.second << ")";
    std::vector<std::pair<double,double>> merged;
    auto current = y_ranges[0];

    for (size_t i = 1; i < y_ranges.size(); ++i)
    {
        auto& next = y_ranges[i];

        if (next.first <= current.second + 1e-2)
        {
            // пересекаются или касаются
            current.second = std::max(current.second, next.second);
        }
        else
        {
            merged.push_back(current);
            current = next;
        }
    }

    merged.push_back(current);
    return merged;
}

double FuncGL::rectangle_integrate(double a, double b)
{
    int n = 1000;
    double h = (b - a) / n;
    double I = 0.0;
    for (int i = 0; i < n; ++i)
    {
        double x = a + (i + 0.5) * h;
        double y;

        if (!eval(x, y) || std::isnan(y))
            continue;

        I += y;
    }
    return I * h;
}

double FuncGL::trapezoid_integrate(double a, double b)
{
    int n = 1000;
    double h = (b - a) / n;
    double I = 0.0;
    for (int i = 0; i < n; ++i)
    {
        double x = a + i * h;
        double x1 = a + (i + 1) * h;
        double y, y1;

        if (!eval(x, y) || std::isnan(y) || !eval(x1, y1) || std::isnan(y1))
            continue;

        I += (y1 + y) * h / 2.0;
    }
    return I;
}

double FuncGL::simpson_integrate(double a, double b)
{
    int n = 1000;
    if (n % 2 != 0) ++n;

    double h = (b - a) / n;

    double x = a;
    double x1 = b;
    double y, y1;

    while (!eval(x, y) || std::isnan(y))
    {
        x += h / 10.0;
    }
    while (!eval(x1, y1) || std::isnan(y1))
    {
        x1 += h / 10.0;
    }

    double I = y + y1;
    for (int i = 1; i < n - 1; ++i)
    {
        double x = a + i * h;
        double y;
        if (!eval(x, y) || std::isnan(y))
            continue;
        I += (i % 2 == 0 ? 2 : 4) * y;
    }
    return I * h / 3.0;
}

double FuncGL::gauss_composite_integrate(double a, double b)
{
    int segments =  100;
    double h = (b - a) / segments;
    double result = 0.0;

    for (int i = 0; i < segments; ++i)
    {
        double left  = a + i * h;
        double right = left + h;

        double part = gauss_integrate(left, right);
        if (std::isnan(part))
            return std::numeric_limits<double>::quiet_NaN();

        result += part;
    }

    return result;
}

double FuncGL::gauss_integrate(double a, double b)
{
    const int n = 5;
    double x[n] = {1.0/3.0*sqrt(5.0-2*sqrt(10.0/7)), 0.0, -1.0/3.0*sqrt(5.0-2*sqrt(10.0/7)), 1.0/3.0*sqrt(5.0+2*sqrt(10.0/7)), -1.0/3.0*sqrt(5.0+2*sqrt(10.0/7))};
    double w[n] = {(322.0+13*sqrt(70))/900.0, 128.0/225.0, (322.0+13*sqrt(70))/900.0, (322.0-13*sqrt(70))/900.0, (322.0-13*sqrt(70))/900.0};
    double c1 = (b - a)/2.0;
    double c2 = (b + a)/2.0;
    double I = 0.0;
    for (int i = 0; i < n; ++i)
    {
        double xi = c1 * x[i] + c2;
        double y; if (!eval(xi, y) || std::isnan(y))
        { continue; }
        I += w[i] * y;
    }
    I *= c1;

    return I;
}

double FuncGL::arc(double x)
{
    const double h = 1e-5;

    double y1, y2;
    if (!eval(x + h, y1) || !eval(x - h, y2))
        return std::numeric_limits<double>::quiet_NaN();

    double dydx = (y1 - y2) / (2.0 * h);
    return std::sqrt(1.0 + dydx * dydx);
}
double FuncGL::curve_length(double a, double b)
{
    int N = 1000;
    double h = (b-a)/N;

    double L = 0.0;
    for (int i = 0; i < N; ++i)
    {
        double left  = a + i * h;
        double right = left + h;

        // Интегрирование Гауссом
        double I = 0.0;
        const int n = 5;
        double x[n] = {1.0/3.0*sqrt(5.0-2*sqrt(10.0/7)), 0.0, -1.0/3.0*sqrt(5.0-2*sqrt(10.0/7)), 1.0/3.0*sqrt(5.0+2*sqrt(10.0/7)), -1.0/3.0*sqrt(5.0+2*sqrt(10.0/7))};
        double w[n] = {(322.0+13*sqrt(70))/900.0, 128.0/225.0, (322.0+13*sqrt(70))/900.0, (322.0-13*sqrt(70))/900.0, (322.0-13*sqrt(70))/900.0};
        double c1 = (right - left)/2.0;
        double c2 = (right + left)/2.0;
        for (int j = 0; j < n; ++j)
        {
            double xj = c1 * x[j] + c2;
            double y = arc(xj);
            if (std::isnan(y))
            { continue; }
            I += w[j] * y;
        }
        I *= c1;

        if (std::isnan(I))
            return std::numeric_limits<double>::quiet_NaN();

        L += I;
    }
    return L;
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
