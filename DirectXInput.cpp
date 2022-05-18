#include "DirectXInput.h"
#include <cassert>
#include <windef.h>

#define DIRECTINPUT_VERSION		0x0800	//DirectInput�̃o�[�W�����w��
#include <dinput.h>

#pragma comment(lib, "dinput8.lib")
#pragma comment(lib,"dxguid.lib")

//DirectInput�̏�����
static IDirectInput8* directInput = nullptr;
static IDirectInputDevice8* keyboard = nullptr;

//�S�L�[�̓��͏�Ԃ��擾����
static BYTE keys[256] = {};
//�S�L�[�̓��͏�Ԃ��擾����
static BYTE oldkeys[256] = {};

void DirectXInput::InputIni(WNDCLASSEX w, HWND hwnd)	//������
{
	HRESULT result;
	//DirectInput�̏�����
	//static IDirectInput8* directInput = nullptr;
	result = DirectInput8Create(
		w.hInstance, DIRECTINPUT_VERSION, IID_IDirectInput8,
		(void**)&directInput, nullptr);
	assert(SUCCEEDED(result));

	//�L�[�{�[�h�f�o�C�X�̐���
	//IDirectInputDevice8* keyboard = nullptr;
	result = directInput->CreateDevice(GUID_SysKeyboard, &keyboard, NULL);
	assert(SUCCEEDED(result));

	//���̓f�[�^�`���̃Z�b�g
	result = keyboard->SetDataFormat(&c_dfDIKeyboard);	//�W���`��
	assert(SUCCEEDED(result));

	//�r�����䃌�x���̃Z�b�g
	result = keyboard->SetCooperativeLevel(
		hwnd, DISCL_FOREGROUND | DISCL_NONEXCLUSIVE | DISCL_NOWINKEY);		//�g���Ă���t���O�ɂ���
	assert(SUCCEEDED(result));												//DISCL_FOREGROUND		��ʂ���O�ɂ���ꍇ�̂ݓ��͂��󂯕t����
																			//DISCL_NONEXCLUSIVE	�f�o�C�X�����̃A�v�������Ő�L���Ȃ�
																			//DISCL_NOWINKEY		Windows�L�[�𖳌������� 
}

void DirectXInput::InputUpdata()	//�A�b�v�f�[�g
{
	for (int i = 0; i < 256; ++i)
	{
		oldkeys[i] = keys[i];
	}
	//�L�[�{�[�h���̎擾�J�n
	keyboard->Acquire();
	keyboard->GetDeviceState(sizeof(keys), keys);
}
//�������ςȂ�
bool DirectXInput::IsKeyDown(char key)
{
	return keys[key];
}
//�������u��
bool DirectXInput::IsKeyTrigger(char key)
{
	return keys[key] && !oldkeys[key];
}
//�������u��
bool DirectXInput::GetKeyReleased(char key)
{
	return !keys[key] && oldkeys[key];
}