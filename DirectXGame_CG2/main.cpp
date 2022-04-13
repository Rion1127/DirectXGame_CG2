#include <Windows.h>

//�E�B���h�E�v���V�[�W��
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

//Windows�A�v���ł̃G���g���[�|�C���g(main�֐�)
int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR, int)
{
	const int Window_width = 1280;	//����
	const int window_height = 720;	//�c��

	//�E�B���h�E�N���X�̐ݒ�
	WNDCLASSEX w{};
	w.cbSize = sizeof(WNDCLASSEX);				
	w.lpfnWndProc = (WNDPROC)WindowProc;		//�E�B���h�E�v���V�[�W����ݒ�
	w.lpszClassName = L"DirectXGame";			//�E�B���h�E�N���X��
	w.hInstance = GetModuleHandle(nullptr);		//�E�B���h�E�n���h��
	w.hCursor = LoadCursor(NULL, IDC_ARROW);	//�J�[�\���w��

	//�E�B���h�E�N���X��OS�ɓo�^����
	RegisterClassEx(&w);
	//�E�B���h�E�T�C�Y{�@X���W�@Y���W�@�����@�c��}
	RECT wrc = { 0,0,Window_width,window_height };
	//�����ŃT�C�Y���C������
	AdjustWindowRect(&wrc, WS_OVERLAPPEDWINDOW, false);

	//�E�B���h�E�I�u�W�F�N�g�̐���
	HWND hwnd = CreateWindow(w.lpszClassName,
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

		//DirectX�����������@��������

		//DirectX�����������@�����܂�

		//�Q�[�����[�v
		while (true) {
			// ���b�Z�[�W������H
			if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
				TranslateMessage(&msg); // �L�[���̓��b�Z�[�W�̏���
				DispatchMessage(&msg); // �v���V�[�W���Ƀ��b�Z�[�W�𑗂�
			}
			// ?�{�^���ŏI�����b�Z�[�W��������Q�[�����[�v�𔲂���
			if (msg.message == WM_QUIT) {
				break;
			}
			// DirectX���t���[������ ��������
			
			// DirectX���t���[������ �����܂�
		}



	//�R���\�[���ւ̕����o��
	//OutputDebugStringA("Hello,DirectX!!\n");
	return 0;
}



