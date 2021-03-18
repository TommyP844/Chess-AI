#pragma once

class Keyboard
{
public: 
	Keyboard() {}
	~Keyboard() {}

protected:
	bool m_keys[1024];
	bool m_Left_CRTL;
	bool m_Right_CRTL;
	bool m_Left_SHIFT;
	bool m_Right_SHIFT;
};
