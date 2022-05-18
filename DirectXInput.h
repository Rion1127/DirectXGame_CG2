#pragma once
#include <dinput.h>

class DirectXInput
{
public:
	static void InputIni(WNDCLASSEX w, HWND hwnd);
	static void InputUpdata();

	static bool IsKeyDown(char key);		//押しっぱなし
	static bool IsKeyTrigger(char key);		//押した瞬間
	static bool GetKeyReleased(char key);

private:

};

