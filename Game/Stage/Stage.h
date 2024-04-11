#pragma once
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Collision/Collision.h"

// �X�e�[�W
class Stage
{
public:
    Stage() {}
    virtual ~Stage() {}

    // �X�V����
    virtual void Update(float elapsedTime) = 0;

    //�`�揈��
    virtual void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader) = 0;

    // ���C�L���X�g
    virtual bool RayCast(const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        HitResult& hit) = 0;

    virtual void DrawDebugGui() {}

    Rect GetStageSize() { return stage_size; }

protected:
    Rect stage_size = {};// �X�e�[�W�͈�
};