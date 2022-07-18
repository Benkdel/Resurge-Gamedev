#include "Application.h"
#include "engine/Window.h"

namespace Amba {

	Application::Application()
		: AB_DeltaTime(0.0f), AB_TimeElapsed(0.0f), m_Window(NULL),
		m_ScrWidth(1280), m_ScrHeight(760)
	{
		Amba::Log::Init();
		AB_TRACE("Application Created Succesfully");
	}

	Application::~Application()
	{
	}

	void Application::Init(const char* title, unsigned int width, unsigned int height)
	{
		m_Window = new Amba::Window("Resurge", width, height);
		m_ScrWidth = width;
		m_ScrHeight = height;
	}

	// method to be override by user
	void Application::OnUserCreate()
	{
	}

	// method to be override by user
	void Application::OnUserUpdate()
	{
	}

	bool Application::AB_keyPressed(unsigned int GLFW_KEY_CODE)
	{
		if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_CODE) == GLFW_PRESS)
			return true;
		else
			return false;
	}

	bool Application::AB_keyReleased(unsigned int GLFW_KEY_CODE)
	{
		if (glfwGetKey(m_Window->getWindow(), GLFW_KEY_CODE) == GLFW_RELEASE)
			return true;
		else
			return false;
	}

	void Application::SetWindowShouldClose()
	{
		m_Window->SetWindowShouldClose();
	}

	void Application::Run()
	{
		double lastTime = 0.0f;

		// method used by user
		OnUserCreate();
	
		while (!m_Window->WindowShouldClose())
		{
			Renderer::Clear();

			// Compute delta time and store it as class member for user to use
			AB_TimeElapsed = glfwGetTime();
			AB_DeltaTime = AB_TimeElapsed - lastTime;
			lastTime = AB_TimeElapsed;
		
			// method used by user
			OnUserUpdate();

			m_Window->OnUpdate();
		}

		delete m_Window;
	}
}
