#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <algorithm>
#include <cstdlib>
#include <ctime>
#include <iostream>
#include <sstream>

const int gridSize = 20;
const int initialSnakeLength = 3;
float baseDelay = 0.15f;
int windowWidth = 600, windowHeight = 600;

struct Point
{
    int x, y;
    bool operator==(const Point &other) const
    {
        return x == other.x && y == other.y;
    }
};

struct Food
{
    Point pos;
    bool isRed;
};

std::vector<Point> snake;
std::vector<Food> foods;
int dirX = 1, dirY = 0;
float lastMoveTime = 0.f;
int score = 0, level = 1;
int nextLevelScore = 50; // score threshold for next level
float moveDelay = baseDelay;

GLuint shaderProgram, vao, vbo;

const char *vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform vec2 offset, scale, resolution;
void main() {
    vec2 pos = (aPos * scale + offset) / resolution * 2.0 - 1.0;
    pos.y = -pos.y;
    gl_Position = vec4(pos, 0.0, 1.0);
}
)glsl";

const char *fragmentShaderSource = R"glsl(
#version 330 core
uniform vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)glsl";

void checkShader(GLuint s)
{
    int ok;
    glGetShaderiv(s, GL_COMPILE_STATUS, &ok);
    if (!ok)
    {
        char log[512];
        glGetShaderInfoLog(s, 512, nullptr, log);
        std::cerr << log << std::endl;
    }
}

GLuint compileShader(GLenum t, const char *src)
{
    GLuint s = glCreateShader(t);
    glShaderSource(s, 1, &src, nullptr);
    glCompileShader(s);
    checkShader(s);
    return s;
}

void setupShaders()
{
    GLuint v = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint f = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, v);
    glAttachShader(shaderProgram, f);
    glLinkProgram(shaderProgram);
    glDeleteShader(v);
    glDeleteShader(f);
}

void setupRect()
{
    float verts[8] = {0, 0, 1, 0, 1, 1, 0, 1};
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(verts), verts, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);
}

void placeFood()
{
    foods.clear();
    Point p;
    do
    {
        p = {rand() % gridSize, rand() % gridSize};
    } while (std::find(snake.begin(), snake.end(), p) != snake.end());
    foods.push_back({p, false}); // always non-red food
}

void resetGame()
{
    snake.clear();
    for (int i = 0; i < initialSnakeLength; i++)
        snake.push_back({10 - i, 10});
    dirX = 1;
    dirY = 0;
    score = 0;
    level = 1;
    moveDelay = baseDelay;
    nextLevelScore = 50; // reset level threshold
    placeFood();
}

void nextLevel()
{
    level++;
    moveDelay *= 0.9f;
}

void update()
{
    Point head = {snake[0].x + dirX, snake[0].y + dirY};
    if (head.x < 0 || head.x >= gridSize || head.y < 0 || head.y >= gridSize ||
        std::find(snake.begin(), snake.end(), head) != snake.end())
    {
        resetGame();
        return;
    }

    snake.insert(snake.begin(), head);
    bool ate = false;
    for (const auto &f : foods)
    {
        if (head == f.pos)
        {
            score += 5; // fixed score for food
            ate = true;
            break;
        }
    }

    if (ate)
    {
        if (score >= nextLevelScore)
        {
            nextLevel();
            nextLevelScore += 50;
        }
        placeFood();
    }
    else
    {
        snake.pop_back();
    }
}

void drawCell(int x, int y, float r, float g, float b)
{
    glUseProgram(shaderProgram);

    float cellSize = std::min(windowWidth, windowHeight) / (float)gridSize;
    float gridPixelWidth = cellSize * gridSize;
    float gridPixelHeight = cellSize * gridSize;

    float offsetX = (windowWidth - gridPixelWidth) / 2.0f;
    float offsetY = (windowHeight - gridPixelHeight) / 2.0f;

    float cellX = offsetX + x * cellSize;
    float cellY = offsetY + y * cellSize;

    glUniform3f(glGetUniformLocation(shaderProgram, "ourColor"), r, g, b);
    glUniform2f(glGetUniformLocation(shaderProgram, "offset"), cellX, cellY);
    glUniform2f(glGetUniformLocation(shaderProgram, "scale"), cellSize, cellSize);
    glUniform2f(glGetUniformLocation(shaderProgram, "resolution"), windowWidth, windowHeight);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
}

void drawGrid()
{
    for (int x = 0; x < gridSize; ++x)
        for (int y = 0; y < gridSize; ++y)
        {
            float shade = ((x + y) % 2 == 0) ? 0.6f : 0.5f;
            drawCell(x, y, 0.0f, shade, 0.0f);
        }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT);

    drawGrid();

    for (const auto &s : snake)
        drawCell(s.x, s.y, 1.0f, 0.0f, 0.0f);

    // Draw blue food only
    for (const auto &f : foods)
        drawCell(f.pos.x, f.pos.y, 0.0f, 0.0f, 1.0f);

    std::stringstream ss;
    ss << "Score: " << score << "   Level: " << level;
    glfwSetWindowTitle(glfwGetCurrentContext(), ss.str().c_str());
}

void key_cb(GLFWwindow *, int k, int, int action, int)
{
    if (action != GLFW_PRESS)
        return;
    if (k == GLFW_KEY_UP && dirY != 1)
    {
        dirX = 0;
        dirY = -1;
    }
    if (k == GLFW_KEY_DOWN && dirY != -1)
    {
        dirX = 0;
        dirY = 1;
    }
    if (k == GLFW_KEY_LEFT && dirX != 1)
    {
        dirX = -1;
        dirY = 0;
    }
    if (k == GLFW_KEY_RIGHT && dirX != -1)
    {
        dirX = 1;
        dirY = 0;
    }
    if (k == GLFW_KEY_R)
        resetGame();
}

void fb_cb(GLFWwindow *, int w, int h)
{
    windowWidth = w;
    windowHeight = h;
    glViewport(0, 0, w, h);
}

int main()
{
    srand((unsigned int)time(nullptr));
    if (!glfwInit())
        return -1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow *win = glfwCreateWindow(windowWidth, windowHeight, "Snake", nullptr, nullptr);
    if (!win)
    {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(win);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    glfwSetKeyCallback(win, key_cb);
    glfwSetFramebufferSizeCallback(win, fb_cb);
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSwapInterval(1);

    setupShaders();
    setupRect();
    resetGame();

    while (!glfwWindowShouldClose(win))
    {
        float t = glfwGetTime();
        if (t - lastMoveTime >= moveDelay)
        {
            update();
            lastMoveTime = t;
        }
        render();
        glfwSwapBuffers(win);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(win);
    glfwTerminate();
    return 0;
}
