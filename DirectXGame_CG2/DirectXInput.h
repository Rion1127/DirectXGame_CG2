#pragma once
#include <dinput.h>

class DirectXInput
{
public:
	static void InputIni(HRESULT result, WNDCLASSEX w, HWND hwnd);
	static void InputUpdata();

	static bool IsKeyDown(char key);		//�������ςȂ�
	static bool IsKeyTrigger(char key);		//�������u��
	static bool GetKeyReleased(char key);

private:

};

