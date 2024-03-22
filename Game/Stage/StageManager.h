#pragma once
#include "Stage.h"

class StageManager
{
private:
    StageManager() {}
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
    void Render(float elapsedTime, ID3D11PixelShader** replaced_pixel_shader);

    // ステージ登録
    void Register(Stage* stage);

    // ステージ全削除
    void Clear();

    // レイキャスト
    bool RayCast(
        const DirectX::XMFLOAT3& start,
        const DirectX::XMFLOAT3& end,
        HitResult& hit
    );

    void DrawDebugGui();

private:
    std::vector<Stage*> stages;
};

