#pragma once

#include "GamePad.h"
#include "Mouse.h"

// �C���v�b�g
class Input
{
public:
    Input(HWND hWnd);
    ~Input() {}

public:
    // �C���X�^���X�擾
    static Input& Instance() { return *instance; }

    // �X�V����
    void Update();

    // �Q�[���p�b�h�擾
    GamePad& GetGamePad() { return gamePad; }

    // �}�E�X�擾
    Mouse& GetMouse() { return mouse; }

    bool GetButtonDownKey(int a);
private:
    static Input* instance;
    GamePad				gamePad;
    Mouse				mouse;
    bool key_flag;
};
