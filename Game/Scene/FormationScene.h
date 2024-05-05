
#pragma once
#include "./Lemur/Scene/BaseScene.h"
#include "./Lemur/Component/GameObject.h"
#include "./Lemur/Component/DemoPlayer.h"
#include "./Lemur/Component/DemoEnemy.h"
#include "./Lemur/Graphics/Shader.h"
#include "./Lemur/Graphics/Texture.h"
#include "./Lemur/Graphics/Framework.h"

// BLOOM
#include "./Lemur/Graphics/Bloom.h"

// Audio
#include <wrl.h>
#include "./Lemur/Audio/audio.h"

#include "./Lemur/Effekseer/Effect.h"

// Font
#include"./Lemur/Font/Font.h"
#include <dwrite.h>                
#pragma comment(lib,"d2d1.lib")    
#pragma comment(lib,"dwrite.lib")

// GLTF
#include "./Lemur/Model/GltfModel.h"

#include "./Lemur/Math/Vector.h"
#include "./Lemur/Math/MathHelper.h"

#include "./Lemur/Model/FbxModelManager.h"
#include "./Lemur/Model/GltfModelManager.h"


class FormationScene :public Lemur::Scene::BaseScene
{
public:
    FormationScene() {}
    ~FormationScene() override {}

    // èâä˙âª
    void Initialize()override;

    // èIóπâª
    void Finalize()override;

    // çXêVèàóù
    void Update(HWND hwnd, float elapsedTime)override;

    // ï`âÊèàóù
    void Render(float elapsedTime)override;

    void DebugImgui()override;

    void InitializeLight()override;
private:
    std::shared_ptr<Sprite> back;
    std::shared_ptr<Sprite> line_1;
    std::shared_ptr<Sprite> line_2;
    std::shared_ptr<Sprite> unit_1;
    std::shared_ptr<Sprite> unit_2;
    std::shared_ptr<Sprite> unit_3;
    std::shared_ptr<Sprite> unit_4;
    std::shared_ptr<Sprite> unit_5;
    std::shared_ptr<Sprite> unit_6;
    std::shared_ptr<Sprite> Button;
    std::shared_ptr<Sprite> base;
    std::shared_ptr<Sprite> Controller_UI_A;
    std::shared_ptr<Sprite> Controller_UI_B;
    std::shared_ptr<Sprite> Controller_UI_X;
    std::shared_ptr<Sprite> Controller_UI_Y;
    std::shared_ptr<Sprite> mark_1;
    std::shared_ptr<Sprite> mark_1_1;
    std::shared_ptr<Sprite> mark_2;
    std::shared_ptr<Sprite> mark_2_2;

    std::unique_ptr<GltfModelManager> gltf_unit_1;
    std::unique_ptr<GltfModelManager> gltf_unit_2;
    std::unique_ptr<GltfModelManager> gltf_unit_3;
    std::unique_ptr<GltfModelManager> gltf_unit_4;
    std::unique_ptr<GltfModelManager> gltf_unit_5;
    std::unique_ptr<GltfModelManager> gltf_unit_6;
};