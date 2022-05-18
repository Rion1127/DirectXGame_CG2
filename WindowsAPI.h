#pragma once
#include <WinUser.h>
#include <windef.h>

class WindowsAPI
{
public:
	WindowsAPI();

	void SetWindowsAPI();

	void MakeWindow();

	void UpdataWindowsAPI();
	
private:
	const int window_width = 1280;	//横幅
	const int window_height = 720;	//縦幅
	//ウィンドウクラスの設定
	WNDCLASSEX w{};
	//ウィンドウサイズ
	RECT wrc;
	//ウィンドウオブジェクトの生成
	HWND hwnd;
};

