#version 330 core
layout (location = 0) in vec3 aPos; // координата вершины
layout (location = 1) in vec3 aColor; // цвет вершины

uniform mat4 model; // матрица модели (поворачивает, масштабирует, сдвигает объект)
uniform mat4 view; // матрица вида - камера (описывает, где находится наблюдатель)
uniform mat4 projection; // матрица проекции (описывает, как 3D переводится в 2D)

out vec3 vertexColor;

void main(void)
{
    gl_Position = projection * view * model * vec4(aPos, 1.0);
    vertexColor = aColor;
}
