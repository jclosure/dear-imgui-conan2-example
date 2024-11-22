#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include <iostream>
#include <vector>
#include <fstream>
#include <sstream>

namespace main_draggable4
{

    // Vertex and index data for a cuboid
    std::vector<float> vertices = {
        -0.5f, -0.5f, -0.5f, 0.5f, -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, -0.5f, 0.5f, -0.5f,
        -0.5f, -0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f, 0.5f, 0.5f, -0.5f, 0.5f, 0.5f};
    std::vector<unsigned int> indices = {
        0, 1, 2, 2, 3, 0, 4, 5, 6, 6, 7, 4,
        4, 5, 1, 1, 0, 4, 3, 2, 6, 6, 7, 3,
        4, 0, 3, 3, 7, 4, 1, 5, 6, 6, 2, 1};

    // Controls
    glm::vec3 translation(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation(0.0f, 0.0f, 0.0f);

    // Read shader file
    std::string ReadShaderFile(const std::string &filepath)
    {
        std::ifstream file(filepath);
        if (!file.is_open())
        {
            std::cerr << "Failed to open shader file: " << filepath << std::endl;
            return "";
        }
        std::stringstream buffer;
        buffer << file.rdbuf();
        return buffer.str();
    }

    // Compile shader
    GLuint CompileShader(GLenum shaderType, const std::string &source)
    {
        GLuint shader = glCreateShader(shaderType);
        const char *src = source.c_str();
        glShaderSource(shader, 1, &src, nullptr);
        glCompileShader(shader);

        GLint success;
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Shader compilation error:\n"
                      << infoLog << std::endl;
            return 0;
        }

        return shader;
    }

    // Create shader program
    GLuint CreateShaderProgram(const std::string &vertexPath, const std::string &fragmentPath)
    {
        std::string vertexSource = ReadShaderFile(vertexPath);
        std::string fragmentSource = ReadShaderFile(fragmentPath);

        if (vertexSource.empty() || fragmentSource.empty())
        {
            std::cerr << "Failed to load shader files." << std::endl;
            return 0;
        }

        GLuint vertexShader = CompileShader(GL_VERTEX_SHADER, vertexSource);
        if (!vertexShader)
            return 0;

        GLuint fragmentShader = CompileShader(GL_FRAGMENT_SHADER, fragmentSource);
        if (!fragmentShader)
            return 0;

        GLuint program = glCreateProgram();
        glAttachShader(program, vertexShader);
        glAttachShader(program, fragmentShader);
        glLinkProgram(program);

        GLint success;
        glGetProgramiv(program, GL_LINK_STATUS, &success);
        if (!success)
        {
            char infoLog[512];
            glGetProgramInfoLog(program, sizeof(infoLog), nullptr, infoLog);
            std::cerr << "Program linking error:\n"
                      << infoLog << std::endl;
            glDeleteShader(vertexShader);
            glDeleteShader(fragmentShader);
            return 0;
        }

        glDeleteShader(vertexShader);
        glDeleteShader(fragmentShader);

        return program;
    }

    // Window resize callback
    void framebuffer_size_callback(GLFWwindow *window, int width, int height)
    {
        glViewport(0, 0, width, height);
    }

    int main()
    {
        // Initialize GLFW and OpenGL
        if (!glfwInit())
        {
            std::cerr << "Failed to initialize GLFW\n";
            return -1;
        }

        // Decide GL+GLSL versions
        // GL 3.2 + GLSL 150
        const char *glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac

        GLFWwindow *window = glfwCreateWindow(800, 600, "Cuboid App", nullptr, nullptr);
        if (!window)
        {
            std::cerr << "Failed to create GLFW window\n";
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        // Initialize GLEW
        if (glewInit() != GLEW_OK)
        {
            std::cerr << "Failed to initialize GLEW\n";
            glfwTerminate();
            return -1;
        }

        // ensures correct rendering of 3d objects when they overlap
        glEnable(GL_DEPTH_TEST);

        glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

        // Set up ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGui_ImplGlfw_InitForOpenGL(window, true);
        ImGui_ImplOpenGL3_Init("#version 330");

        GLuint shaderProgram = CreateShaderProgram("vertex-shader.glsl", "fragment-shader.glsl");
        if (!shaderProgram)
            return -1;

        // OpenGL Buffers
        GLuint VAO, VBO, EBO;
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(1, &EBO);

        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.data(), GL_STATIC_DRAW);

        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void *)0);
        glEnableVertexAttribArray(0);

        while (!glfwWindowShouldClose(window))
        {
            glfwPollEvents();

            // Render UI
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            ImGui::Begin("Object Controls");
            ImGui::SliderFloat3("Translation", &translation.x, -1.0f, 1.0f);
            ImGui::SliderFloat3("Rotation", &rotation.x, 0.0f, 360.0f);
            static bool enableDepthTest = true;
            if (ImGui::Checkbox("Enable Depth Test", &enableDepthTest))
            {
                if (enableDepthTest)
                    glEnable(GL_DEPTH_TEST);
                else
                    glDisable(GL_DEPTH_TEST);
            }

            if (ImGui::Button("Reset Transform"))
            {
                translation = glm::vec3(0.0f);
                rotation = glm::vec3(0.0f);
            }

            ImGui::End();

            ImGui::Render();

            // Render Scene
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            glUseProgram(shaderProgram);

            glm::mat4 model = glm::translate(glm::mat4(1.0f), translation);
            model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
            model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

            glm::mat4 view = glm::lookAt(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f), glm::vec3(0.0f, 1.0f, 0.0f));
            glm::mat4 projection = glm::perspective(glm::radians(45.0f), 800.0f / 600.0f, 0.1f, 100.0f);

            // get the uniform locations
            GLint modelLoc = glGetUniformLocation(shaderProgram, "model");
            GLint viewLoc = glGetUniformLocation(shaderProgram, "view");
            GLint projLoc = glGetUniformLocation(shaderProgram, "projection");

            // ensure they all exist in our program
            if (modelLoc == -1 || viewLoc == -1 || projLoc == -1)
            {
                std::cerr << "Error: Uniform not found in shader program!" << std::endl;
                return -1;
            }

            // bind our model, view, and projection matrices
            glUniformMatrix4fv(modelLoc, 1, GL_FALSE, &model[0][0]);
            glUniformMatrix4fv(viewLoc, 1, GL_FALSE, &view[0][0]);
            glUniformMatrix4fv(projLoc, 1, GL_FALSE, &projection[0][0]);

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);

            // Render ImGui
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
        }

        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(1, &EBO);

        glfwDestroyWindow(window);
        glfwTerminate();

        return 0;
    }

} // namespace main_draggable4
