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

    // •`‰æˆ—
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    void DrawDebugGUI();

    float GetTimer() { return timer; };
    int GetNumber() { return number; };
    void SetNumber(int number_) { number= number_; };
    void DecNumber() { number--; }

private:
    float timer;
    int number;
};


