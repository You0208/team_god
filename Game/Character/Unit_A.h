#pragma once
#include "Unit.h"
#include "Lemur/Model/FbxModelManager.h"

class Unit_A :public Unit
{
public:
    Unit_A();
    ~Unit_A()override;

    void Update(float elapsedTime)override;

    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) override;

    void DrawDebugGUI();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive();

private:

protected:
private:
    // �A�j���[�V����
    enum Animation
    {
        Anim_Attack,
        Anim_Death,
        Anim_Falling,
        Anim_GetHit1,
        Anim_GetHit2,
        Anim_Idle,
        Anim_Jump,
        Anim_Jump_Flip,
        Anim_Landing,
        Anim_Revive,
        Anim_Running,
        Anim_Walking
    };

private:
    std::shared_ptr<FbxModelManager> model_unit_A;
    float moveSpeed = 5.0f;
    float turnSpeed = DirectX::XMConvertToRadians(720);
    float jumpSpeed = 20.0f;

    int jumpCount = 0;
    int jumpLimit = 2;

    float LuanchRate = 0.1f;
    float LuanchCollTime = 0.1f;

    float leftHandRadius = 0.4f;

    // �U�������蔻�肷�邩
    bool attackCollisionFlag = false;
};


