#pragma once
#include "Stage.h"

class StageManager
{
private:
    StageManager(){ result_health_parsent = 0.0f; }
    ~StageManager() {}

public:
    // 唯一のインスタンス取得
    static StageManager& Instance()
    {
        static StageManager instance;
        return instance;
    }

    // 更新処理
    void update(float elapsedTime);

    // 描画処理
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader, ID3D11PixelShader** replaced_pixel_shader2);

    // ステージ登録
    void Register(Stage* stage);

    // ステージ全削除
    void Clear();

    void DrawDebugGui();

    Stage* GetStage(int j) { return stages.at(j); }
    int GetStageIndex() { return stage_index; }
    int GetStageLevel() { return stage_level; }
    void SetStageIndex(int stage_index_) { stage_index= stage_index_; }
    void SetStageLevel (int stage_level_) { stage_level = stage_level_; }

    void AddStageLevel() { stage_level++; }
public:
    float result_health_parsent = 0.0f;
private:
    std::vector<Stage*> stages;
    int stage_index;
    int stage_level;
};

