#pragma once
#include "GameObject.h"
#include "Component.h"
#include "../Model/Model.h"
#include "../Graphics/Graphics.h"
#include "../Resource/ResourceManager.h"
#include"../Input/Input.h"

class DemoEnemy :public GameObject
{
public:
    DemoEnemy() {}
    DemoEnemy(InputComponent* input_,
        PhysicsComponent* physics_,
        GraphicsComponent* graphics_) :GameObject(input_, physics_, graphics_) {}
};

class DemoEnemyInputComponent :public InputComponent
{
    void Initialize(GameObject* gameobj) override {}
    void Update(GameObject* gameobj, float elapsedTime) override;
};

class DemoEnemyPhysicsComponent :public PhysicsComponent
{
    void Initialize(GameObject* gameobj) override {}
    void Update(GameObject* gameobj, float elapsedTime) override {}
};

class DemoEnemyGraphicsComponent :public GraphicsComponent
{
    void Initialize(GameObject* gameobj) override;
    void Update(GameObject* gameobj) override;
    void Render(GameObject* gameobj, float elapsedTime, ID3D11PixelShader* replaced_pixel_shader, bool shadow = false) override;
private:
    std::shared_ptr<SkinnedMesh> DemoEnemyModel;
};