#pragma once

// API's
#include <SDL\SDL_keyboard.h>
#include <SDL\SDL_mouse.h>
#include <SDL\SDL.h>

#include "ImGui/imgui.h"
#include "ImGui/imconfig.h"
#include "ImGui/imgui_impl_opengl3.h"
#include "ImGui/imgui_internal.h"
#include "ImGui/imstb_rectpack.h"
#include "ImGui/imstb_textedit.h"
#include "ImGui/imstb_truetype.h"	
#include "ImGui/imgui_impl_sdl.h"


#include "keyboard.h"
#include "cam2d.h"

// Standard Headers
#include <iostream>
#include <string>
#include <glm\glm.hpp>


class Window : public Keyboard
{
public:
	Window(const std::string& title, int mWidth, int mHeight);
	virtual ~Window();
	
	bool is_open() { return !isClosed; };

	void startFrame();
	void endFrame();

	glm::ivec2 GetMousePos() const { return m_mousePos; }
	glm::vec2 GetWindowSize() const { return glm::vec2(mWidth, mHeight); }
	bool GetLeftIsPressed() const { return m_leftIsPressed; }


private:
	void GetEvent();
	void swap_buffers();
	void begin_imgui_window();
	void end_imgui_window();

	glm::ivec2 m_mousePos;
	bool isClosed;
	bool m_leftIsPressed;
	int mWidth, mHeight;	
	
	SDL_Window* m_window;
	SDL_GLContext m_glContext;

};

