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


    float GetTimer() { return timer; };
    int GetNumber() { return number; };
    bool GetBorn() { return born; };
    void SetNumber(int number_) { number= number_; };
    void SetCategory(int category_) { category = category_; };
    void SetBorn(bool born_) { born = born_; };
    void DecNumber() { number--; }

private:
    float   timer;   // 種が配置されてからの時間
    int     number;  // 種番号
    int     category;// 種の種類
    bool    born;    // ユニットが生まれるかどうか
};


