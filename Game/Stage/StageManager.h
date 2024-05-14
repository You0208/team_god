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

    void DrawDebugGui();

    Stage* GetStage(int j) { return stages.at(j); }
    int GetStageIndex() { return stage_index; }
    int GetStageLevel() { return stage_level; }
    void SetStageIndex(int stage_index_) { stage_index= stage_index_; }
    void SetStageLevel (int stage_level_) { stage_level = stage_level_; }

    void AddStageLevel() { stage_level++; }
private:
    std::vector<Stage*> stages;
    int stage_index;
    int stage_level;
};

