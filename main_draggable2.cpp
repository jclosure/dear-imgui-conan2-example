#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace main_draggable2
{
    float objectX = 0.0f, objectY = 0.0f; // Object position
    float delta = 0.9f;
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

    int main()
    {
        // Initialize GLFW
        if (!glfwInit())
            return -1;

        GLFWwindow *window = glfwCreateWindow(800, 600, "Drag and Drop", NULL, NULL);
        if (!window)
        {
            glfwTerminate();
            return -1;
        }
        glfwMakeContextCurrent(window);

        // Set callbacks
        glfwSetMouseButtonCallback(window, mouseButtonCallback);
        glfwSetCursorPosCallback(window, cursorPosCallback);

        while (!glfwWindowShouldClose(window))
        {
            glClear(GL_COLOR_BUFFER_BIT);

            // Update quad vertices based on object position
            quadVertices = {
                glm::vec3(objectX - 0.1f, objectY - 0.1f, 0.0f),
                glm::vec3(objectX + 0.1f, objectY - 0.1f, 0.0f),
                glm::vec3(objectX + 0.1f, objectY + 0.1f, 0.0f),
                glm::vec3(objectX - 0.1f, objectY + 0.1f, 0.0f)};

            // Render the object
            glBegin(GL_QUADS);
            for (const auto &vertex : quadVertices)
            {
                glVertex3f(vertex.x, vertex.y, vertex.z);
            }
            glEnd();

            glfwSwapBuffers(window);
            glfwPollEvents();
        }

        glfwDestroyWindow(window);
        glfwTerminate();
        return 0;
    }
}