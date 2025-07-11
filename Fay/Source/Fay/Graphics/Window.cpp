#include <Graphics/Window.h>
namespace Fay {
	void window_resize(GLFWwindow* window, int width, int height);
	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);
	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos);

	Window::Window(const char* title, int width, int height)
	{
		m_title = title;
		m_width = width;
		m_height = height;

		if (!init())
			glfwTerminate();

		for (int i = 0; i < MAX_KEYS; i++)
		{
			m_keys[i] = false;
			m_keyState[i] = false;
		}

		for (int i = 0; i < MAX_BUTTONS; i++)
		{
			m_mouseButtons[i] = false;
		}
	}


	Window::~Window()
	{
		glfwTerminate();
	}

	bool Window::init()
	{
		if (!glfwInit())
		{
			std::cout << "Failed to initialize GLFW!" << std::endl;
			return false;
		}
		m_window = glfwCreateWindow(m_width, m_height, m_title, NULL, NULL);
		if (!m_window)
		{
			std::cout << "Failed to create GLFW window!" << std::endl;
			return false;
		}
		glfwMakeContextCurrent(m_window);
		glfwSetWindowUserPointer(m_window, this);
		glfwSetFramebufferSizeCallback(m_window, window_resize);
		glfwSetKeyCallback(m_window, key_callback);
		glfwSetMouseButtonCallback(m_window, mouse_button_callback);
		glfwSetCursorPosCallback(m_window, cursor_position_callback);
		glfwSwapInterval(1.0); // 0 is disabled 1 is enabled;
			if (glewInit() != GLEW_OK)
			{
				std::cout << "Could not initialize glew" << std::endl;
				return false;
			}
			glEnable(GL_BLEND);
			glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
			std::cout << "OpenGL " << glGetString(GL_VERSION) << std::endl;
			return true;
	}
	void Window::clear() const
	{
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}
	void Window::update()
	{
		for (int i = 0; i < MAX_KEYS; i++)
		{
			m_keyState[i] = m_keys[i];
		}

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
			std::cout << "OpenGL Error: " << error << std::endl;
		glfwPollEvents();
		glfwSwapBuffers(m_window);
	}

	bool Window::closed() const
	{
		return glfwWindowShouldClose(m_window) == 1;
	}

	bool Window::isKeyPressed(unsigned int keycode)
	{
		// TODO: Log this
		if (keycode >= MAX_KEYS)
			return false;
		return m_keys[keycode];
	}
	bool Window::isMouseButtonPressed(unsigned int button)
	{
		// TODO: Log this
		if (button >= MAX_BUTTONS)
			return false;
		return m_mouseButtons[button];
	}

	void Window::getMousePos(double& x, double& y)
	{
		x = mx;
		y = my;
	}


	void window_resize(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_width = width;
		win->m_height = height;
	}

	void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_keys[key] = action != GLFW_RELEASE;
	}

	void mouse_button_callback(GLFWwindow* window, int button, int action, int mods)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->m_mouseButtons[button] = action != GLFW_RELEASE;
	}

	void cursor_position_callback(GLFWwindow* window, double xpos, double ypos)
	{
		Window* win = (Window*)glfwGetWindowUserPointer(window);
		win->mx = xpos;
		win->my = ypos;
	}

}