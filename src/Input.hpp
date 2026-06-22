#pragma once
#include <windows.h>

class Input
{
  public:
    static void Initialize();
    static void Update();
    static bool IsKeyPressed(unsigned char key);  // Just pressed this frame
    static bool IsKeyReleased(unsigned char key); // Just released this frame
    static bool IsKeyDown(unsigned char key);     // Currently held down (NEW)

  private:
    static bool keys[256];
    static bool keysPrev[256];
    static void SetKeyState(unsigned char key, bool state);
};
