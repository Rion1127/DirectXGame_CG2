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
	const int window_width = 1280;	//����
	const int window_height = 720;	//�c��
	//�E�B���h�E�N���X�̐ݒ�
	WNDCLASSEX w{};
	//�E�B���h�E�T�C�Y
	RECT wrc;
	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd;
};

