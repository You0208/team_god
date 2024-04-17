#pragma once
#include "Lemur/Object/Character.h"
#include "Lemur/Graphics/Shader.h"
#include "Lemur/Model/FbxModelManager.h"
#include "Lemur/Effekseer/Effect.h"
class Seed :public Character
{
public:
    Seed();
    ~Seed()override;

    void Update(float elapsedTime);

    // 描画処理
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    void DrawDebugGUI();

    // デバッグプリミティブ描画
    void DrawDebugPrimitive();

    // 飛ばしたての種の位置決定
    void DecisionPos();

    // ランダムなベクトル
    DirectX::XMFLOAT2 GenerateRandomDirection();

    // 指定した距離分だけ移動する関数
    DirectX::XMFLOAT2 MoveInRandomDirection(const DirectX::XMFLOAT2& position, float distance);


    float   GetTimer() { return timer; };
    int     GetNumber() { return number; };
    bool    GetBorn() { return born; };
    bool    GetDesitionPos() { return decision_pos; }

    void    SetNumber(int number_) { number= number_; };
    void    SetCategory(int category_) { category = category_; };
    void    SetBorn(bool born_) { born = born_; };
    void    SetDesitionPos(bool decision_pos_) { decision_pos = decision_pos_; }

    void    DecNumber() { number--; }

private:
    float               timer;          // 種が配置されてからの時間
    int                 number;         // 種番号
    int                 category;       // 種の種類
    bool                born;           // ユニットが生まれるかどうか
    bool                decision_pos;   // ユニットの位置が決定されているか
    int                 overlap_num;    // 何回重なったか
    DirectX::XMFLOAT2   outPosition{};  // 押し出し位置
};


