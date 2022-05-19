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

	//�E�B���h�E�N���X�̐ݒ�
	WNDCLASSEX w{};
	//�E�B���h�E�T�C�Y{�@X���W�@Y���W�@�����@�c��}
	RECT wrc;
	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd;
	//���b�Z�[�W
	MSG msg{};

private:
	const int window_width = 1280;	//����
	const int window_height = 720;	//�c��


};


