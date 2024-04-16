#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
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

    // Imgui
    void DrawDebugGui();

    // �f�o�b�O�v���~�e�B�u�`��
    void DrawDebugPrimitive()override;

    // �C���X�^���X�擾
    static Fence& Instance();

    Rect GetLeftRect() { return left_rect; }
    Rect GetFrontRect() { return front_rect; }
    Rect GetBackRect() { return back_rect; }

private:    
    // �f�o�b�O�p�Ɏl�p�g���Ă�
    Rect left_rect      = {};
    Rect front_rect     = {};
    Rect back_rect      = {};   // ��͂Ȃ����Ǔ����蔻��p��
    Rect right_rect = {};   // ��͂Ȃ����Ǔ����蔻��p��
    float rect_width    = 0.0f;
};