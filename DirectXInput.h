#pragma once
#include <dinput.h>

class DirectXInput
{
public:
	static void InputIni(WNDCLASSEX w, HWND hwnd);
	static void InputUpdata();

	static bool IsKeyDown(UINT8 key);		//‰Ÿ‚µ‚Á‚Ï‚È‚µ
	static bool IsKeyTrigger(UINT8 key);		//‰Ÿ‚µ‚½uŠÔ
	static bool GetKeyReleased(UINT8 key);

private:

};

