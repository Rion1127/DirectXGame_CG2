#include "DirectXInput.h"
#include <cassert>
#include <windef.h>

#define DIRECTINPUT_VERSION		0x0800	//DirectInputのバージョン指定
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//DirectInputの初期化
static IDirectInput8* directInput = nullptr;
static IDirectInputDevice8* keyboard = nullptr;

//全キーの入力状態を取得する
static BYTE keys[256] = {};
//全キーの入力状態を取得する
static BYTE oldkeys[256] = {};

void DirectXInput::InputIni(WNDCLASSEX w, HWND hwnd)	//初期化
{
	HRESULT result;
	//DirectInputの初期化
	//static IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//キーボードデバイスの生成
	//IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//入力データ形式のセット
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);	//標準形式
	assert(SUCCEEDED(result));

	//排他制御レベルのセット
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);		//使っているフラグについて
	assert(SUCCEEDED(result));												//DISCL_FOREGROUND		画面が手前にある場合のみ入力を受け付ける
																			//DISCL_NONEXCLUSIVE	デバイスをこのアプリだけで占有しない
																			//DISCL_NOWINKEY		Windowsキーを無効化する 
}

void DirectXInput::InputUpdata()	//アップデート
{
	for (int i = 0; i < 256; ++i)
	{
		oldkeys[i] = keys[i];
	}
	//キーボード情報の取得開始
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(keys), keys);
}
//押しっぱなし
bool DirectXInput::IsKeyDown(char key)
{
	return keys[key];
}
//押した瞬間
bool DirectXInput::IsKeyTrigger(char key)
{
	return keys[key] && !oldkeys[key];
}
//離した瞬間
bool DirectXInput::GetKeyReleased(char key)
{
	return !keys[key] && oldkeys[key];
}