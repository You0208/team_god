#include "Input.h"

Input* Input::instance = nullptr;

// コンストラクタ
Input::Input(HWND hWnd)
    : mouse(hWnd)
{
    instance = this;
}

// 更新処理
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
            //キーを押した瞬間の処理
            key_flag = true;
        }
        //キーを押している間の処理
    }
    if (!GetAsyncKeyState(a) & 0x8000) 
    {
        //キーを離している間の処理
        key_flag = false;
    }
    return key_flag;
}
