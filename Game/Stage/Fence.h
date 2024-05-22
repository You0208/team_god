#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Collision/Collision.h"
#include "Lemur/Math/MathHelper.h"

// �X�e�[�W
class Fence :public Character
{
private:

public:
    Fence();
    ~Fence()override;

    // �X�V����
    void Update(float elapsedTime);

    //�`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    void FenceShake(float elapsedTime);
    void CallFenceShake();
    void StopFenceShake();

    // Imgui
    void DrawDebugGui();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

    // �C���X�^���X�擾
    static Fence& Instance();

    Rect GetLeftRect() { return left_rect; }
    Rect GetFrontRect() { return front_rect; }
    Rect GetBackRect() { return back_rect; }
    Rect GetRightRect() { return right_rect; }

    float health_prsent = 0.0f;
    int fence_state = 0;
    EasingFunction fence1_pos = {};
    EasingFunction fence2_pos = {};
    enum FENCE_STATE
    {
        FANCE_0,
        FANCE_1,
        FANCE_2,
    };
private:    
    // �f�o�b�O�p�Ɏl�p�g���Ă�
    Rect left_rect      = {};
    Rect front_rect     = {};
    Rect back_rect      = {};   // ��͂Ȃ����Ǔ����蔻��p��
    Rect right_rect     = {};   // ��͂Ȃ����Ǔ����蔻��p��
    float rect_width    = 0.0f; 

};