#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Effekseer/Effect.h"
#include "Seed.h"

class Player :public Character
{
public:
    Player();
    ~Player()override;

    // 更新処理
    void Update(float elapsedTime);

    // Imgui
    void DrawDebugGUI();

    // はじき処理
    void Flick(float elapsedTime);

    // インスタンス取得
    static Player& Instance();
private:
    // 入力処理
    void InputProcess();

private:

    float moveSpeed = 5.0f;// 横移動の速度
    float sub_pos_z = -5.0f;// 減算されるZ座標

    int unit_category = 0;// 選択されているユニットの種類

    // コントローラー用
    float s_l;// コントローラーの右スティックY
    float timer_s;// コントローラーがはじかれるのにかかった時間
    float s_l_max;//はじかれた右スティックY座標の最大 
    float f_d = 0;// 最終的に導き出されたはじき座標


    DirectX::XMFLOAT2 limit;
};


