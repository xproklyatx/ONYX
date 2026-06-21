#pragma once
class Input
{
public:
	static void Initialize();
	static void Update();
	static bool IsKeyPressed(unsigned char key);
	static bool IsKeyReleased(unsigned char key);
private:
	static bool keys[256];          
	static bool keysPrev[256];       
	static void SetKeyState(unsigned char key, bool state);
};
