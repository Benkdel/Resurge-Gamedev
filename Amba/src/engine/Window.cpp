#include "Window.h"

#include <engine/Log.h>
#include <engine/core.h>

#include <engine/io/Keyboard.h>
#include <engine/io/Mouse.h>

#ifdef AB_ENABLE_ASSERTS
	#include <engine/OpenGlErrorChecking.h>
#endif

namespace Amba {

	static bool frameBufferCallbackCalled = false;

	void FramebufferSizeCallback(GLFWwindow* window, int width, int height)
	{
		glViewport(0, 0, width, height);
		frameBufferCallbackCalled = true;
	}

	Window::Window(const std::string& title, unsigned int width, unsigned int height)
	{
		m_Width = width;
		m_Height = height;
		m_Title = title;

		m_WindowShouldClose = false;

		AB_INFO("Creating window {0} ({1}, {2})", title, width, height);
	
		if (!s_GLFWInitialized)
		{
			int success = glfwInit();
			AB_ASSERT(success, "Could not initialize GLFW!\n");

			s_GLFWInitialized = true;
		}

		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

		m_Window = glfwCreateWindow(m_Width, m_Height, m_Title.c_str(), NULL, NULL);
		if (!m_Window)
		{
			AB_ASSERT(0, "Could not create Window!\n");
		}
		glfwMakeContextCurrent(m_Window);

		// set callbacks
		glfwSetFramebufferSizeCallback(m_Window, FramebufferSizeCallback);
		glfwSetKeyCallback(m_Window, KeyBoard::KeyCallback);
		glfwSetCursorPosCallback(m_Window, Mouse::CursorPosCallback);
		glfwSetMouseButtonCallback(m_Window, Mouse::MouseButtonCallback);
		glfwSetScrollCallback(m_Window, Mouse::MouseWheelCallback);

		// hide cursor
		glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		m_IsCursorLocked = 0;

		// initialize GLAD
		int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		AB_ASSERT(success, "Could not load GLAD!\n");

		// enable depth testing and blending
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_BLEND);

#ifdef AB_ENABLE_ASSERTS
		// only in debug mode, enable output and 
		// set callback
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
		glDebugMessageCallback((GLDEBUGPROC)MessageCallback, NULL);
#endif
	}

	Window::~Window() {}

	ViewPortData Window::OnUpdate()
	{
		glfwSwapBuffers(m_Window);
		glfwPollEvents();


		// Update windows size
		if (frameBufferCallbackCalled)
		{
			int width = 0;
			int height = 0;
			glfwGetWindowSize(m_Window, &width, &height);
			m_Width = width;
			m_Height = height;
			return ViewPortData({ m_Width, m_Height, true });
		}

		return ViewPortData({ m_Width, m_Height, false });

	}

	void Window::SetWindowShouldClose()
	{
		m_WindowShouldClose = true;
	}

	void Window::ToggleCursor()
	{
		if (m_IsCursorLocked)
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
			m_IsCursorLocked = 0;
		}
		else
		{
			glfwSetInputMode(m_Window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
			m_IsCursorLocked = 1;
		}
	}

	void Window::ShutDown()
	{
		m_WindowShouldClose = true;
		if (m_Window != NULL)
		{
			glfwDestroyWindow(m_Window);
		}
	}
}
