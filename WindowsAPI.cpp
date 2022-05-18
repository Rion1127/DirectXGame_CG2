#include "WindowsAPI.h"

LRESULT WindowProc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
	//���b�Z�[�W�ɉ����ăQ�[�����L�̏������s��
	switch (msg) {
		//�E�B���h�E���j�����ꂽ
	case WM_DESTROY:
		//OS�ɑ΂��āA�A�v���̏I����`����
		return 0;
	}
	//�W���̃��b�Z�[�W�������s��
	return DefWindowProc(hwnd, msg, wparam, lparam);
}

WindowsAPI::WindowsAPI()
{
	
}

void WindowsAPI::SetWindowsAPI() {
	w.cbSize = sizeof(WNDCLASSEX);
	w.lpfnWndProc = (WNDPROC)WindowProc;		//�E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectXGame";			//�E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr);		//�E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//�J�[�\���w��
	//�E�B���h�E�N���X��OS�ɓo�^����
	RegisterClassEx(&w);
	//�E�B���h�E�T�C�Y{�@X���W�@Y���W�@�����@�c��}
	wrc = { 0,0,window_width,window_height };
	//�����ŃT�C�Y���C������
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

	//�E�B���h�E��\����Ԃɂ���
	ShowWindow(hwnd, SW_SHOW);

	MSG msg{};		//���b�Z�[�W
}

void WindowsAPI::UpdataWindowsAPI() {
	//�E�B���h�E�̐���(�ݒ�)
	SetWindowsAPI();
	//�E�B���h�E�̐���(����)
	MakeWindow();
}
