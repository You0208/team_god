#pragma once
#include "Stage.h"

class StageManager
{
private:
    StageManager() {}
    ~StageManager() {}

public:
    // �B��̃C���X�^���X�擾
    static StageManager& Instance()
    {
        static StageManager instance;
        return instance;
    }

    // �X�V����
    void update(float elapsedTime);

    // �`�揈��
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // �X�e�[�W�o�^
    void Register(Stage* stage);

    // �X�e�[�W�S�폜
    void Clear();

    // ���C�L���X�g
    bool RayCast(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        HitResult& hit
    );

    void DrawDebugGui();

private:
    std::vector<Stage*> stages;
};

