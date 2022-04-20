#pragma once
#include <dinput.h>

class DirectXInput
{
public:
	static void InputIni(HRESULT result, WNDCLASSEX w, HWND hwnd);
	static void InputUpdata();

	static bool IsKeyDown(char key);		//‰Ÿ‚µ‚Á‚Ï‚È‚µ
	static bool IsKeyTrigger(char key);		//‰Ÿ‚µ‚½uŠÔ
	static bool GetKeyReleased(char key);

private:

};

