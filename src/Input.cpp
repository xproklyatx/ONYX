#include "Input.hpp"
#include <windows.h>
#include <cstring>
bool Input::keys[256] = { false };
bool Input::keysPrev[256] = { false };
void Input::Initialize() 
{
	std::memset(keys, false, sizeof(keys));
	std::memset(keysPrev, false, sizeof(keysPrev));
}
void Input::Update()
{
	std::memcpy(keysPrev, keys, sizeof(keys));
	for (int i = 0; i < 256; ++i)
	{
		SetKeyState(i, (GetAsyncKeyState(i) & 0x8000) != 0);
	}
}
void Input::SetKeyState(unsigned char key, bool state)
{
	if (state != keys[key]) {
		keys[key] = state;
	}
}
bool Input::IsKeyPressed(unsigned char key)
{
	return keys[key] && !keysPrev[key]; 
}
bool Input::IsKeyReleased(unsigned char key)
{
	return !keys[key] && keysPrev[key];  
}
