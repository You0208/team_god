#include "Input.h"

Input* Input::instance = nullptr;

// �R���X�g���N�^
Input::Input(HWND hWnd)
    : mouse(hWnd)
{
    instance = this;
}

// �X�V����
void Input::Update()
{
    gamePad.Update();
    mouse.Update();
}

bool Input::GetButtonDownKey(int a)
{

    if (!key_flag)
    {
        if (GetAsyncKeyState(a) & 0x8000)
        {
            //�L�[���������u�Ԃ̏���
            key_flag = true;
        }
        //�L�[�������Ă���Ԃ̏���
    }
    if (!GetAsyncKeyState(a) & 0x8000) 
    {
        //�L�[�𗣂��Ă���Ԃ̏���
        key_flag = false;
    }
    return key_flag;
}
