#pragma once
#include <Windows.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <cassert>
#include <vector>
#include <string>
#include <DirectXMath.h>
using namespace DirectX;
#include <d3dcompiler.h>
#pragma comment(lib, "d3dcompiler.lib")

#pragma comment(lib, "d3d12.lib")
#pragma comment(lib, "dxgi.lib")

class WinAPI
{
public:

	void SetWindowsAPI();

	void MakeWindow();

	void UpdataWindowsAPI();

	int GetWinWidth();

	int GetWinHeight();

	//ウィンドウクラスの設定
	WNDCLASSEX w{};
	//ウィンドウサイズ{　X座標　Y座標　横幅　縦幅}
	RECT wrc;
	//ウィンドウオブジェクトの生成
	HWND hwnd;
	//メッセージ
	MSG msg{};

private:
	const int window_width = 1280;	//横幅
	const int window_height = 720;	//縦幅


};


