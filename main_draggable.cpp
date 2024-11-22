#include <GLFW/glfw3.h>
#include <GL/glew.h>
#include <glm/glm.hpp>
#include <iostream>

namespace main_draggable
{
    // Global variables to track the mouse position
    float last_x = 0.0f, last_y = 0.0f; // Last position when the mouse is pressed
    bool is_dragging = false;           // Flag to check if dragging is active
    glm::vec3 triangle_vertices[3] = {
        glm::vec3(-0.5f, -0.5f, 0.0f),
        glm::vec3(0.5f, -0.5f, 0.0f),
        glm::vec3(0.0f, 0.5f, 0.0f)};

    // Function to handle drag
    void handle_drag(double xpos, double ypos, bool mouse_pressed)
    {
        if (mouse_pressed)
        {
            if (!is_dragging)
            {
                // Start dragging: record the initial position
                last_x = xpos;
                last_y = ypos;
                is_dragging = true;
            }
            else
            {
                // While dragging, calculate the mouse movement
                float dx = xpos - last_x;
                float dy = ypos - last_y;

                // Update triangle vertices by the amount the mouse moved
                for (int i = 0; i < 3; i++)
                {
                    triangle_vertices[i] += glm::vec3(dx * 0.01f, dy * -0.01f, 0.0f); // scale the movement
                }

                // Update last position for next movement
                last_x = xpos;
                last_y = ypos;
            }
        }
        else
        {
            is_dragging = false; // Stop dragging when the mouse is released
        }
    }

    // Shader source code (vertex and fragment shaders)
    const char *vertex_shader_source = R"(
    #version 330 core
    layout(location = 0) in vec3 aPos;
    void main() {
        gl_Position = vec4(aPos, 1.0);
    }
)";

    const char *fragment_shader_source = R"(
    #version 330 core
    out vec4 FragColor;
    void main() {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue color
    }
)";

    GLuint vao, vbo, shader_program;

    // Initialize OpenGL, create VBO, VAO, and shaders
    void setup()
    {
        // Compile and link shaders
        GLuint vertex_shader = glCreateShader(GL_VERTEX_SHADER);
        glShaderSource(vertex_shader, 1, &vertex_shader_source, nullptr);
        glCompileShader(vertex_shader);

        GLuint fragment_shader = glCreateShader(GL_FRAGMENT_SHADER);
        glShaderSource(fragment_shader, 1, &fragment_shader_source, nullptr);
        glCompileShader(fragment_shader);

        shader_program = glCreateProgram();
        glAttachShader(shader_program, vertex_shader);
        glAttachShader(shader_program, fragment_shader);
        glLinkProgram(shader_program);

        // Generate and bind VAO and VBO
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        // Load the triangle vertices into the buffer
        glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_DYNAMIC_DRAW);

        // Setup vertex attributes (position)
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        glBindVertexArray(0); // Unbind VAO
    }

    // Render function to draw the triangle
    void render()
    {
        // Update the VBO with the new triangle positions after each frame
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(triangle_vertices), triangle_vertices);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // Clear the screen
        glClear(GL_COLOR_BUFFER_BIT);
        glUseProgram(shader_program);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        glBindVertexArray(0);
    }

    int main()
    {
        // Initialize GLFW
        if (!glfwInit())
        {
            std::cerr << "GLFW initialization failed!" << std::endl;
            return -1;
        }

        // Set GLFW window hint to use OpenGL 3.2 core profile (needed on macOS)
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac

        // Create GLFW window
        GLFWwindow *window = glfwCreateWindow(800, 600, "OpenGL Drag Triangle", nullptr, nullptr);
        if (!window)
        {
            std::cerr << "Failed to create GLFW window!" << std::endl;
            glfwTerminate();
            return -1;
        }

        // Make the OpenGL context current
        glfwMakeContextCurrent(window);
        glfwSwapInterval(1); // Enable vsync

        // Initialize GLEW (must be called after making the context current)
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "GLEW initialization failed!" << std::endl;
            return -1;
        }

        // Setup OpenGL resources (VAO, VBO, shaders)
        setup();

        // Set GLFW callbacks
        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double xpos, double ypos)
                                 {
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            handle_drag(xpos, ypos, true);
        } else {
            handle_drag(xpos, ypos, false);
        } });

        // Main loop
        while (!glfwWindowShouldClose(window))
        {
            render();

            // Poll for events
            glfwPollEvents();
            glfwSwapBuffers(window);
        }

        // Cleanup and exit
        glfwTerminate();
        return 0;
    }
}