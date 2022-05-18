#include "WindowsAPI.h"

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//メッセージに応じてゲーム特有の処理を行う
	switch (msg) {
		//ウィンドウが破棄された
	case WM_DESTROY:
		//OSに対して、アプリの終了を伝える
		return 0;
	}
	//標準のメッセージ処理を行う
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WindowsAPI::WindowsAPI()
{
	
}

void WindowsAPI::SetWindowsAPI() {
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;		//ウィンドウプロシージャを設定
	w.lpszClassName = L"DirectXGame";			//ウィンドウクラス名
	w.hInstance = GetModuleHandle(nullptr);		//ウィンドウハンドル
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//カーソル指定
	//ウィンドウクラスをOSに登録する
	RegisterClassEx(&w);
	//ウィンドウサイズ{　X座標　Y座標　横幅　縦幅}
	wrc = { 0,0,window_width,window_height };
	//自動でサイズを修正する
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);
}

void WindowsAPI::MakeWindow() {
	hwnd = CreateWindow(w.lpszClassName,
		L"DirectXGame",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		wrc.right - wrc.left,
		wrc.bottom - wrc.top,
		nullptr,
		nullptr,
		w.hInstance,
		nullptr);

	//ウィンドウを表示状態にする
	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};		//メッセージ
}

void WindowsAPI::UpdataWindowsAPI() {
	//ウィンドウの生成(設定)
	SetWindowsAPI();
	//ウィンドウの生成(生成)
	MakeWindow();
}
