#pragma once
#include "GameObject.h"
#include "Component.h"
#include "../Model/Model.h"
#include "../Graphics/Graphics.h"
#include "../Resource/ResourceManager.h"
#include"../Input/Input.h"

class DemoPlayer :public GameObject
{
public:
    DemoPlayer() {}
    DemoPlayer(InputComponent* input_,
        PhysicsComponent* physics_,
        GraphicsComponent* graphics_) :GameObject(input_, physics_, graphics_) {}

    void DebugImgui()override;
    void DrawDebugPrimitive()override;
};

class DemoPlayerInputComponent :public InputComponent
{
    void Initialize(GameObject* gameobj) override {}
    void Update(GameObject* gameobj, float elapsedTime) override;
};

class DemoPlayerPhysicsComponent :public PhysicsComponent
{
    void Initialize(GameObject* gameobj) override;
    void Update(GameObject* gameobj, float elapsedTime) override;
};

class DemoPlayerGraphicsComponent:public GraphicsComponent
{
    void Initialize(GameObject* gameobj) override;
    void Update(GameObject* gameobj) override;
    void Render(GameObject* gameobj, float elapsedTime,ID3D11PixelShader* replaced_pixel_shader, bool shadow = false) override;
private:
    std::shared_ptr<SkinnedMesh> DemoPlayerModel;
};