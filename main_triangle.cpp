#include "imgui.h"
#include "bindings/imgui_impl_glfw.h"
#include "bindings/imgui_impl_opengl3.h"
#include "opengl_shader.h"
#include "file_manager.h"
#include <stdio.h>
#include <iostream>
#include <vector>

#include <GL/glew.h> // Initialize with glewInit()

// Include glfw3.h after our OpenGL definitions
#include <GLFW/glfw3.h>

namespace main_triangle
{

#define PI 3.14159265358979323846

	static void glfw_error_callback(int error, const char *description)
	{
		fprintf(stderr, "Glfw Error %d: %s\n", error, description);
	}

	void render_conan_logo()
	{
		ImDrawList *draw_list = ImGui::GetWindowDrawList();
		float sz = 300.0f;
		static ImVec4 col1 = ImVec4(68.0 / 255.0, 83.0 / 255.0, 89.0 / 255.0, 1.0f);
		static ImVec4 col2 = ImVec4(40.0 / 255.0, 60.0 / 255.0, 80.0 / 255.0, 1.0f);
		static ImVec4 col3 = ImVec4(50.0 / 255.0, 65.0 / 255.0, 82.0 / 255.0, 1.0f);
		static ImVec4 col4 = ImVec4(20.0 / 255.0, 40.0 / 255.0, 60.0 / 255.0, 1.0f);
		const ImVec2 p = ImGui::GetCursorScreenPos();
		float x = p.x + 4.0f, y = p.y + 4.0f;
		draw_list->AddQuadFilled(ImVec2(x, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y + 0.5 * sz), ImVec2(x + sz, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y), ImColor(col1));
		draw_list->AddQuadFilled(ImVec2(x, y + 0.25 * sz), ImVec2(x + 0.5 * sz, y + 0.5 * sz), ImVec2(x + 0.5 * sz, y + 1.0 * sz), ImVec2(x, y + 0.75 * sz), ImColor(col2));
		draw_list->AddQuadFilled(ImVec2(x + 0.5 * sz, y + 0.5 * sz), ImVec2(x + sz, y + 0.25 * sz), ImVec2(x + sz, y + 0.75 * sz), ImVec2(x + 0.5 * sz, y + 1.0 * sz), ImColor(col3));
		draw_list->AddLine(ImVec2(x + 0.75 * sz, y + 0.375 * sz), ImVec2(x + 0.75 * sz, y + 0.875 * sz), ImColor(col4));
		draw_list->AddBezierCubic(ImVec2(x + 0.72 * sz, y + 0.24 * sz), ImVec2(x + 0.68 * sz, y + 0.15 * sz), ImVec2(x + 0.48 * sz, y + 0.13 * sz), ImVec2(x + 0.39 * sz, y + 0.17 * sz), ImColor(col4), 10, 18);
		draw_list->AddBezierCubic(ImVec2(x + 0.39 * sz, y + 0.17 * sz), ImVec2(x + 0.2 * sz, y + 0.25 * sz), ImVec2(x + 0.3 * sz, y + 0.35 * sz), ImVec2(x + 0.49 * sz, y + 0.38 * sz), ImColor(col4), 10, 18);
	}

	void create_triangle(unsigned int &vbo, unsigned int &vao, unsigned int &ebo)
	{

		// create the triangle
		float triangle_vertices[] = {
			0.0f, 0.25f, 0.0f,	  // position vertex 1
			1.0f, 0.0f, 0.0f,	  // color vertex 1
			0.25f, -0.25f, 0.0f,  // position vertex 1
			0.0f, 1.0f, 0.0f,	  // color vertex 1
			-0.25f, -0.25f, 0.0f, // position vertex 1
			0.0f, 0.0f, 1.0f,	  // color vertex 1
		};
		unsigned int triangle_indices[] = {
			0, 1, 2};
		glGenVertexArrays(1, &vao);
		glGenBuffers(1, &vbo);
		glGenBuffers(1, &ebo);
		glBindVertexArray(vao);
		glBindBuffer(GL_ARRAY_BUFFER, vbo);
		glBufferData(GL_ARRAY_BUFFER, sizeof(triangle_vertices), triangle_vertices, GL_STATIC_DRAW);
		glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
		glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(triangle_indices), triangle_indices, GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)0);
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void *)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, 0);
		glBindVertexArray(0);
	}

	// Variables for tracking dragging state
	static bool dragging = false;
	static ImVec2 drag_start(0.0f, 0.0f);
	static float translation[] = {0.0f, 0.0f};

	// Function to check if a point is inside the triangle
	bool is_point_in_triangle(float px, float py, float ax, float ay, float bx, float by, float cx, float cy)
	{
		// Compute vectors and areas for the hit test
		float v0x = cx - ax, v0y = cy - ay;
		float v1x = bx - ax, v1y = by - ay;
		float v2x = px - ax, v2y = py - ay;

		float dot00 = v0x * v0x + v0y * v0y;
		float dot01 = v0x * v1x + v0y * v1y;
		float dot02 = v0x * v2x + v0y * v2y;
		float dot11 = v1x * v1x + v1y * v1y;
		float dot12 = v1x * v2x + v1y * v2y;

		float denom = dot00 * dot11 - dot01 * dot01;
		if (denom == 0.0f)
			return false;

		float inv_denom = 1.0f / denom;
		float u = (dot11 * dot02 - dot01 * dot12) * inv_denom;
		float v = (dot00 * dot12 - dot01 * dot02) * inv_denom;

		return (u >= 0) && (v >= 0) && (u + v < 1);
	}

	void handle_drag(GLFWwindow *window, float &translation_x, float &translation_y)
	{
		static bool dragging = false;
		static float drag_start_x = 0.0f, drag_start_y = 0.0f;

		// Get mouse button state and position
		double mouse_x, mouse_y;
		glfwGetCursorPos(window, &mouse_x, &mouse_y);

		int window_width, window_height;
		glfwGetFramebufferSize(window, &window_width, &window_height);

		// Convert to OpenGL's normalized device coordinates
		float mouse_ndc_x = (mouse_x / window_width) * 2.0f - 1.0f;
		float mouse_ndc_y = 1.0f - (mouse_y / window_height) * 2.0f;

		// Triangle vertices in NDC
		float ax = translation_x + 0.0f, ay = translation_y + 0.25f;  // Top vertex
		float bx = translation_x + 0.25f, by = translation_y - 0.25f; // Bottom-right vertex
		float cx = translation_x - 0.25f, cy = translation_y - 0.25f; // Bottom-left vertex

		if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS)
		{
			if (!dragging)
			{
				// Start dragging if the mouse is over the triangle
				if (is_point_in_triangle(mouse_ndc_x, mouse_ndc_y, ax, ay, bx, by, cx, cy))
				// if (true)
				{
					dragging = true;
					drag_start_x = mouse_ndc_x - translation_x;
					drag_start_y = mouse_ndc_y - translation_y;
				}
			}
			else
			{
				// Update translation while dragging
				translation_x = mouse_ndc_x - drag_start_x;
				translation_y = mouse_ndc_y - drag_start_y;
			}
		}
		else
		{
			dragging = false;
		}
	}

	// renamed
	int main()
	{
		// Setup window
		glfwSetErrorCallback(glfw_error_callback);
		if (!glfwInit())
			return 1;

		// Decide GL+GLSL versions
#if __APPLE__
		// GL 3.2 + GLSL 150
		const char *glsl_version = "#version 150";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE); // 3.2+ only
		glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);		   // Required on Mac
#else
		// GL 3.0 + GLSL 130
		const char *glsl_version = "#version 130";
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
		// glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
		// glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

		// Create window with graphics context
		GLFWwindow *window = glfwCreateWindow(1280, 720, "Dear ImGui - Conan", NULL, NULL);
		if (window == NULL)
			return 1;
		glfwMakeContextCurrent(window);
		glfwSwapInterval(1); // Enable vsync

		bool err = glewInit() != GLEW_OK;

		if (err)
		{
			fprintf(stderr, "Failed to initialize OpenGL loader!\n");
			return 1;
		}

		int screen_width, screen_height;
		glfwGetFramebufferSize(window, &screen_width, &screen_height);
		glViewport(0, 0, screen_width, screen_height);

		// create our geometries
		unsigned int vbo, vao, ebo;
		create_triangle(vbo, vao, ebo);

		// init shader
		Shader triangle_shader;
		triangle_shader.init(FileManager::read("simple-shader.vs"), FileManager::read("simple-shader.fs"));

		// Setup Dear ImGui context
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();
		ImGuiIO &io = ImGui::GetIO();
		// Setup Platform/Renderer bindings
		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init(glsl_version);
		// Setup Dear ImGui style
		ImGui::StyleColorsDark();

		while (!glfwWindowShouldClose(window))
		{
			glfwPollEvents();
			glClearColor(0.45f, 0.55f, 0.60f, 1.00f);
			glClear(GL_COLOR_BUFFER_BIT);

			handle_drag(window, translation[0], translation[1]);

			// Render triangle
			triangle_shader.use();
			triangle_shader.setUniform("translation", translation[0], translation[1]);
			glBindVertexArray(vao);
			glDrawElements(GL_TRIANGLES, 3, GL_UNSIGNED_INT, 0);
			glBindVertexArray(0);

			// Render ImGui
			ImGui_ImplOpenGL3_NewFrame();
			ImGui_ImplGlfw_NewFrame();
			ImGui::NewFrame();

			ImGui::Begin("Triangle Position");

			// rotation
			static float rotation = 0.0;
			ImGui::SliderFloat("rotation", &rotation, 0, 2 * PI);
			// pass the parameters to the shader
			triangle_shader.setUniform("rotation", rotation);

			// translation is tracked outside of here
			ImGui::SliderFloat2("Position", translation, -1.0f, 1.0f);
			triangle_shader.setUniform("translation", translation[0], translation[1]);

			// color
			static float color[4] = {1.0f, 1.0f, 1.0f, 1.0f};
			// color picker
			ImGui::ColorEdit3("color", color);
			// multiply triangle's color with this color
			triangle_shader.setUniform("color", color[0], color[1], color[2]);

			ImGui::End();

			ImGui::Render();
			ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
		}

		// Cleanup
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();

		glfwDestroyWindow(window);
		glfwTerminate();

		return 0;
	}

}