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

    // �`�揈��
    void Render(float scale, ID3D11PixelShader** replaced_pixel_shader);

    void DrawDebugGUI();

    // �C���X�^���X�擾
    static Seed& Instance();
private:
    std::shared_ptr<FbxModelManager> model_seed;


};


