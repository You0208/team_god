#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Model/FbxModelManager.h"
#include "Lemur/Effekseer/Effect.h"
class Player :public Character
{
public:
    Player();
    ~Player()override;

    void Update(float elapsedTime);

    // 描画処理
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    void DrawDebugGUI();

    // インスタンス取得
    static Player& Instance();
private:
    void InputProcess(float elapsed_time);
private:
    std::shared_ptr<FbxModelManager> model_player;
    float moveSpeed = 5.0f;
    float speed = {};
    Effect* hitEffect = nullptr;
};


