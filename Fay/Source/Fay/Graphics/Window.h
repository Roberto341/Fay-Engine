	#pragma once
#include <iostream>
#include <glew.h>
#include "../ImGui/imgui.h"
#include "../ImGui/backends/imgui_impl_glfw.h"
#include "../ImGui/backends/imgui_impl_opengl3.h"
#include <glfw3.h>
#include <Utils/MapEditor.h>
#define USING_VIEWPORT 0
namespace Fay {
#define MAX_KEYS 1024
#define MAX_BUTTONS 32
	class Window
	{
	private:
		const char* m_title;
		int m_width, m_height;
		bool m_closed;
		GLFWwindow *m_window;

		bool m_keys[MAX_KEYS];
		bool m_mouseButtons[MAX_BUTTONS];
		bool m_keyState[MAX_KEYS];
		double mx, my;

		// Framebuffer
		GLuint m_fbo = 0;
		GLuint m_renderTexture = 0;
		//GLuint m_rbo = 0; // optional for depth stencil
	public:
		Window(const char* title, int width, int height);
		~Window();
		void clear() const;
		void update();
		bool closed() const;
		// Framebuffer stuff
		void createRenderTaget(int width, int height);
		void bindRenderTarget();
		void unbindRenderTarget();
		inline int getWidth() const { return m_width; }
		inline int getHeight() const { return m_height; }

		GLFWwindow* getWindow() const { return m_window; }
		bool isKeyPressed(unsigned int keycode);
		bool isMouseButtonPressed(unsigned int button);
		void getMousePos(double& x, double& y);
	private:
		bool init();
		friend void window_resize(GLFWwindow* window, int width, int height);
		friend void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
		friend void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
		friend void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	};
}