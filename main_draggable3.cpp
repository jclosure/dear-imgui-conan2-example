#include <GLFW/glfw3.h> // Will drag system OpenGL headers

#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"

namespace main_draggable3
{
    // Quad properties
    glm::vec3 translation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 rotation = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 scale = glm::vec3(1.0f, 1.0f, 1.0f);
    glm::vec4 color = glm::vec4(0.0f, 0.8f, 0.8f, 1.0f);

    float objectX = 0.0f, objectY = 0.0f; // Object position
    bool isDragging = false;

    // Vertex data for the quad
    std::vector<glm::vec3> quadVertices = {
        glm::vec3(-0.1f, -0.1f, 0.0f),
        glm::vec3(0.1f, -0.1f, 0.0f),
        glm::vec3(0.1f, 0.1f, 0.0f),
        glm::vec3(-0.1f, 0.1f, 0.0f)};

    // MVP matrix (identity for simplicity)
    glm::mat4 mvpMatrix = glm::mat4(1.0f);

    // Project a 3D point into 2D screen coordinates using the MVP matrix
    glm::vec2 project_to_ndc(const glm::vec3 &vertex, const glm::mat4 &mvpMatrix)
    {
        glm::vec4 clipSpace = mvpMatrix * glm::vec4(vertex, 1.0f);
        glm::vec3 ndc = glm::vec3(clipSpace) / clipSpace.w; // Perspective divide
        return glm::vec2(ndc.x, ndc.y);
    }

    // Check if a point (xpos, ypos) is inside the quad in normalized device coordinates (NDC)
    bool is_point_inside_3d_object(double xpos, double ypos, const std::vector<glm::vec3> &vertices, const glm::mat4 &mvpMatrix)
    {
        // Project all vertices to NDC
        std::vector<glm::vec2> projectedVertices;
        for (const auto &vertex : vertices)
        {
            projectedVertices.push_back(project_to_ndc(vertex, mvpMatrix));
        }

        // Convert mouse position to NDC
        float x_ndc = (xpos / 400.0f) - 1.0f; // assuming window width = 800
        float y_ndc = 1.0f - (ypos / 300.0f); // assuming window height = 600
        glm::vec2 point_ndc(x_ndc, y_ndc);

        // Ray-casting algorithm to determine if the point is inside the quad
        int n = projectedVertices.size();
        bool inside = false;

        for (int i = 0, j = n - 1; i < n; j = i++)
        {
            const glm::vec2 &v1 = projectedVertices[i];
            const glm::vec2 &v2 = projectedVertices[j];

            bool intersects = ((v1.y > point_ndc.y) != (v2.y > point_ndc.y)) &&
                              (point_ndc.x < (v2.x - v1.x) * (point_ndc.y - v1.y) / (v2.y - v1.y) + v1.x);
            if (intersects)
            {
                inside = !inside;
            }
        }

        return inside;
    }

    // Mouse button callback
    void mouseButtonCallback(GLFWwindow *window, int button, int action, int mods)
    {
        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                // Check if mouse is over the object to start dragging
                double xpos, ypos;
                glfwGetCursorPos(window, &xpos, &ypos);
                if (is_point_inside_3d_object(xpos, ypos, quadVertices, mvpMatrix))
                {
                    isDragging = true;
                }
            }
            else if (action == GLFW_RELEASE)
            {
                isDragging = false;
            }
        }
    }

    // Mouse motion callback
    void cursorPosCallback(GLFWwindow *window, double xpos, double ypos)
    {
        if (isDragging)
        {
            // Update object position based on mouse position
            objectX = (xpos / 400.0f) - 1.0f; // Normalize to NDC
            objectY = 1.0f - (ypos / 300.0f); // Normalize to NDC
        }
    }

    // Render the quad with transformations
    void drawQuad()
    {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
        model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
        model = glm::translate(model, translation);
        model = glm::scale(model, scale);

        glColor4f(color.r, color.g, color.b, color.a);

        glBegin(GL_QUADS);
        for (const auto &vertex : quadVertices)
        {
            glm::vec4 transformedVertex = model * glm::vec4(vertex, 1.0f);
            glVertex3f(transformedVertex.x, transformedVertex.y, transformedVertex.z);
        }
        glEnd();
    }

    int main()
    {

        // Initialize GLFW
        if (!glfwInit())
            return -1;

        // Decide GL+GLSL versions
        // GL 3.2 + GLSL 150
        const char *glsl_version = "#version 150";
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
        glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);           // Required on Mac

        GLFWwindow *window = glfwCreateWindow(800, 600, "ImGui Widget for Quad", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        // Initialize ImGui
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();

        ImGui_ImplOpenGL3_Init(glsl_version);

        ImGui_ImplGlfw_InitForOpenGL(window, true);

        // Set callbacks
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);

        while (!glfwWindowShouldClose(window))
        {
            // Start new ImGui frame
            ImGui_ImplOpenGL3_NewFrame();
            ImGui_ImplGlfw_NewFrame();
            ImGui::NewFrame();

            // ImGui widget
            ImGui::Begin("Quad Controls");
            ImGui::SliderFloat3("Translation", &translation.x, -1.0f, 1.0f);
            ImGui::SliderFloat3("Rotation", &rotation.x, 0.0f, 360.0f);
            ImGui::SliderFloat3("Scale", &scale.x, 0.1f, 3.0f);
            // ImGui::ColorEdit4("Color", &color.r);
            ImGui::End();

            // Clear and render
            glClear(GL_COLOR_BUFFER_BIT);
            drawQuad();

            // Render ImGui
            ImGui::Render();
            ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        // Cleanup ImGui
        ImGui_ImplOpenGL3_Shutdown();
        ImGui_ImplGlfw_Shutdown();
        ImGui::DestroyContext();

        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }

}