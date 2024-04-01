#pragma once
#include "Unit.h"

class Unit_D :public Unit
{
public:
    Unit_D();
    ~Unit_D()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

private:

    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);
    float jumpSpeed = 20.0f;

    // �U�������蔻�肷�邩
    bool attackCollisionFlag = false;
};


