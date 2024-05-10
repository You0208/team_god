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

class OverScene :public Lemur::Scene::BaseScene
{
public:
    OverScene() {}
    ~OverScene() override {}

    // èâä˙âª
    void Initialize()override;

    // èIóπâª
    void Finalize()override;

    // çXêVèàóù
    void Update(HWND hwnd, float elapsedTime)override;

    // ï`âÊèàóù
    void Render(float elapsedTime)override;

    void DebugImgui()override;
private:
    std::shared_ptr<Sprite> game_over_back;
    std::shared_ptr<Sprite> continue_back;
    std::shared_ptr<Sprite> game_over_text;
    std::shared_ptr<Sprite> again_text;
    std::shared_ptr<Sprite> stageselect_text;

    int select_num = 0;

    EasingFunction select;
    EasingFunction again;

    bool once_only = true;
};