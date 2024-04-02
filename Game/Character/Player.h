#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Model/FbxModelManager.h"
#include "Lemur/Effekseer/Effect.h"
#include "Seed.h"

class Player :public Character
{
public:
    Player();
    ~Player()override;

    // 更新処理
    void Update(float elapsedTime);

    // 描画処理
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    // Imgui
    void DrawDebugGUI();

    // はじき処理
    void Flick(float elapsedTime);

    // 種の更新処理
    void SeedUpdate(float elapsedTime);

    // インスタンス取得
    static Player& Instance();
private:
    // 入力処理
    void InputProcess();

private:
    std::vector<Seed*> seeds;// 種のリスト
    float seed_limit_time = 15.0f;// 種の制限時間
    int seed_number = 0;// 次の種番号

    float moveSpeed = 5.0f;// 横移動の速度
    float sub_pos_z = -5.0f;// 減算されるZ座標

    int unit_category = 0;// 選択されているユニットの種類

    // コントローラー用
    float s_l;// コントローラーの右スティックY
    float timer_s;// コントローラーがはじかれるのにかかった時間
    float s_l_max;//はじかれた右スティックY座標の最大 
    float f_d = 0;// 最終的に導き出されたはじき座標
};


