#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <iostream>

// ==== Config ====
const int gridSize = 20;
const float cellSize = 2.0f / gridSize;
float moveDelay = 0.15f;

// ==== Game State ====
struct Point
{
    int x, y;
    bool operator==(const Point &other) const
    {
        return x == other.x && y == other.y;
    }
};

std::vector<Point> snake;
Point food;
int dirX = 1, dirY = 0;
float lastMoveTime = 0.0f;
int score = 0;

// ==== OpenGL State ====
GLuint shaderProgram, vao, vbo;

// ==== Vertex Shader ====
const char *vertexShaderSource = R"glsl(
#version 330 core
layout(location = 0) in vec2 aPos;
uniform vec2 offset;
uniform float scale;
void main() {
    vec2 pos = aPos * scale + offset;
    gl_Position = vec4(pos, 0.0, 1.0);
}
)glsl";

// ==== Fragment Shader ====
const char *fragmentShaderSource = R"glsl(
#version 330 core
uniform vec3 ourColor;
out vec4 FragColor;
void main() {
    FragColor = vec4(ourColor, 1.0);
}
)glsl";

// ==== Compile Shader ====
GLuint compileShader(GLenum type, const char *src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);
    int success;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        char log[512];
        glGetShaderInfoLog(shader, 512, nullptr, log);
        std::cerr << "Shader compile error: " << log << "\n";
    }
    return shader;
}

// ==== Setup Shaders ====
void setupShaders()
{
    GLuint vShader = compileShader(GL_VERTEX_SHADER, vertexShaderSource);
    GLuint fShader = compileShader(GL_FRAGMENT_SHADER, fragmentShaderSource);
    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vShader);
    glDeleteShader(fShader);
}

// ==== Setup Rectangle Geometry ====
void setupRectangle()
{
    float vertices[] = {
        0.0f, 0.0f,
        1.0f, 0.0f,
        1.0f, 1.0f,
        0.0f, 1.0f};

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

// ==== Draw a Cell ====
void drawCell(int x, int y, float r, float g, float b)
{
    glUseProgram(shaderProgram);

    GLint colorLoc = glGetUniformLocation(shaderProgram, "ourColor");
    GLint offsetLoc = glGetUniformLocation(shaderProgram, "offset");
    GLint scaleLoc = glGetUniformLocation(shaderProgram, "scale");

    glUniform3f(colorLoc, r, g, b);
    glUniform2f(offsetLoc, -1.0f + x * cellSize, -1.0f + y * cellSize);
    glUniform1f(scaleLoc, cellSize);

    glBindVertexArray(vao);
    glDrawArrays(GL_TRIANGLE_FAN, 0, 4);
    glBindVertexArray(0);
}

// ==== Food ====
void placeFood()
{
    while (true)
    {
        food = {rand() % gridSize, rand() % gridSize};
        bool valid = true;
        for (const auto &part : snake)
            if (food == part)
                valid = false;
        if (valid)
            break;
    }
}

// ==== Reset ====
void resetGame()
{
    snake = {{10, 10}, {9, 10}, {8, 10}};
    dirX = 1;
    dirY = 0;
    score = 0;
    moveDelay = 0.15f;
    placeFood();
    std::cout << "Game Reset. Score: 0\n";
}

// ==== Update ====
void update()
{
    Point newHead = {snake[0].x + dirX, snake[0].y + dirY};

    // Wall Collision
    if (newHead.x < 0 || newHead.y < 0 || newHead.x >= gridSize || newHead.y >= gridSize)
    {
        resetGame();
        return;
    }

    // Self Collision
    for (const auto &part : snake)
        if (newHead == part)
        {
            resetGame();
            return;
        }

    snake.insert(snake.begin(), newHead);

    if (newHead == food)
    {
        score += 10;
        moveDelay *= 0.97f;
        placeFood();
        std::cout << "Score: " << score << "\n";
    }
    else
    {
        snake.pop_back();
    }
}

// ==== Render ====
void render()
{
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto &part : snake)
        drawCell(part.x, part.y, 0.0f, 1.0f, 0.0f);
    drawCell(food.x, food.y, 1.0f, 0.0f, 0.0f);
}

// ==== Input (FIXED here) ====
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    if (key == GLFW_KEY_UP && dirY != -1)
    {
        dirX = 0;
        dirY = 1;
    }
    else if (key == GLFW_KEY_DOWN && dirY != 1)
    {
        dirX = 0;
        dirY = -1;
    }
    else if (key == GLFW_KEY_LEFT && dirX != 1)
    {
        dirX = -1;
        dirY = 0;
    }
    else if (key == GLFW_KEY_RIGHT && dirX != -1)
    {
        dirX = 1;
        dirY = 0;
    }
}

// ==== Main ====
int main()
{
    srand((unsigned int)time(0));
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    GLFWwindow *window = glfwCreateWindow(600, 600, "Snake Game (Modern OpenGL)", nullptr, nullptr);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        return -1;

    glfwSetKeyCallback(window, key_callback);
    glViewport(0, 0, 600, 600);
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glfwSwapInterval(1); // vsync

    setupShaders();
    setupRectangle();
    resetGame();

    while (!glfwWindowShouldClose(window))
    {
        float now = glfwGetTime();
        if (now - lastMoveTime >= moveDelay)
        {
            update();
            lastMoveTime = now;
        }

        render();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}