#pragma once
#include <dinput.h>

class DirectXInput
{
public:
	static void InputIni(WNDCLASSEX w, HWND hwnd);
	static void InputUpdata();

	static bool IsKeyDown(UINT8 key);		//押しっぱなし
	static bool IsKeyTrigger(UINT8 key);		//押した瞬間
	static bool GetKeyReleased(UINT8 key);

private:

};

