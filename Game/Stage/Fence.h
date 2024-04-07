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

    Rect left_rect;
    Rect front_rect;
};