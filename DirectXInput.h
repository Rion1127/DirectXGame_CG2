#pragma once
#include <dinput.h>

class DirectXInput
{
public:
	static void InputIni(WNDCLASSEX w, HWND hwnd);
	static void InputUpdata();

	static bool IsKeyDown(UINT8 key);		//�������ςȂ�
	static bool IsKeyTrigger(UINT8 key);		//�������u��
	static bool GetKeyReleased(UINT8 key);

private:

};

