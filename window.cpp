// Local Headers
#define GLEW_EXPERIMENTAL
#define GLEW_STATIC

#include "window.h"
#include <GL/glew.h>
#include "GL/GL.h"

Window::Window(const std::string& title, int _width, int _height)
{
	mWidth = _width;
	mHeight = _height;

	SDL_Init(SDL_INIT_EVERYTHING);

	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 2);
	SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_COMPATIBILITY);
	SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_ALPHA_SIZE, 8);
	SDL_GL_SetAttribute(SDL_GL_BUFFER_SIZE, 32);
	SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 16);
	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, true);
	


	m_window = SDL_CreateWindow(title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, mWidth, mHeight, 0 | SDL_WINDOW_OPENGL | SDL_WINDOW_ALLOW_HIGHDPI);
	//SDL_SetWindowResizable(m_window, SDL_FALSE);
	m_glContext = SDL_GL_CreateContext(m_window);

	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Could not initialize OpenGL" << std::endl;
		system("pause");
		exit(1);
	}
		
	SDL_GL_SetSwapInterval(true);
	SDL_SetWindowResizable(m_window, SDL_TRUE);

	glViewport(0, 0, mWidth, mHeight); 

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();

	ImGui::GetIO().ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	const char* glsl_version = "#version 150";
	
	// Setup Platform/Renderer bindings
	ImGui_ImplSDL2_InitForOpenGL(m_window, m_glContext);
	ImGui_ImplOpenGL3_Init(glsl_version);

	// Setup Style
	ImGui::StyleColorsDark();
}

Window::~Window()
{
	ImGui::DestroyContext();
	SDL_GL_DeleteContext(m_glContext);
	SDL_DestroyWindow(m_window);
	SDL_Quit();
}

void Window::GetEvent()
{
	SDL_Event window_event;

	SDL_GetWindowSize(m_window, &mWidth, &mHeight);

	while (SDL_PollEvent(&window_event) != 0)
	{	
		switch (window_event.type)
		{
		case SDL_QUIT:
			isClosed = true;
			break;

		case SDL_MOUSEMOTION:
			SDL_GetMouseState(&m_mousePos.x, &m_mousePos.y);
			break;


		case SDL_MOUSEBUTTONDOWN:
			if (window_event.button.button == SDL_BUTTON_LEFT)
				m_leftIsPressed = true;
			break;

		case SDL_MOUSEBUTTONUP:
			if (window_event.button.button == SDL_BUTTON_LEFT)
				m_leftIsPressed = false;
			break;


		}
	}
}

void Window::swap_buffers()
{
	SDL_GL_SwapWindow(m_window);
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	GetEvent();
}

void Window::startFrame()
{
	begin_imgui_window();
	GetEvent();
}

void Window::endFrame()
{
	end_imgui_window();
	swap_buffers();
}

void Window::begin_imgui_window()
{
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplSDL2_NewFrame(m_window);
	ImGui::NewFrame();
}

void Window::end_imgui_window()
{
	ImGui::Render();
	SDL_GL_MakeCurrent(m_window, m_glContext);
	glViewport(0, 0, (int)ImGui::GetIO().DisplaySize.x, (int)ImGui::GetIO().DisplaySize.y);
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

