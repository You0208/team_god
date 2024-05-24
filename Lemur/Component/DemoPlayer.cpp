#include "DemoPlayer.h"
#include "DemoEnemyManager.h"
#include "../Graphics/Framework.h"
#include "../Graphics/DebugRenderer.h"

void DemoPlayerGraphicsComponent::Initialize(GameObject* gameobj)
{
	DemoPlayer* demoPlayer = dynamic_cast<DemoPlayer*> (gameobj);
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
	demoPlayer->SetModel(ResourceManager::Instance().LoadModelResource(graphics.GetDevice(), ".\\resources_2\\Model\\Jummo\\Jummo.fbx"));
	//demoPlayer->SetModel(ResourceManager::Instance().LoadModelResource(graphics.GetDevice(), ".\\resources\\Stage\\wall.fbx"));
}

void DemoPlayerGraphicsComponent::Update(GameObject* gameobj)
{
}

void DemoPlayerGraphicsComponent::Render(GameObject* gameobj,float elapsedTime,ID3D11PixelShader* replaced_pixel_shader, bool shadow )
{
	DemoPlayer* demoPlayer = dynamic_cast<DemoPlayer*> (gameobj);

	if (shadow)
	{
		ID3D11PixelShader* null_pixel_shader{ NULL };
		demoPlayer->Render(elapsedTime, &null_pixel_shader);
	}
	else
	{
		demoPlayer->Render(elapsedTime, replaced_pixel_shader);
	}
	demoPlayer->DrawDebugPrimitive();
}

// 入力処理
void DemoPlayerInputComponent::Update(GameObject* gameobj, float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	float ax = gamePad.GetAxisRX();
	float ay = gamePad.GetAxisRY();

	float lx = gamePad.GetAxisLX();
	float ly = gamePad.GetAxisLY();
}

void DemoPlayerPhysicsComponent::Initialize(GameObject* gameobj)
{
	DemoPlayer* demoPlayer = dynamic_cast<DemoPlayer*> (gameobj);
	demoPlayer->SetScale({ 0.8f,0.8f,0.8f });
}

void DemoPlayerPhysicsComponent::Update(GameObject* gameobj, float elapsedTime)
{
	DemoPlayer* demoPlayer = dynamic_cast<DemoPlayer*> (gameobj);
	//TODO IMGUI
	demoPlayer->DebugImgui();
}

void DemoPlayer::DebugImgui()
{
	// 左のintはmesh_index、右のintはbone_indexです
	//// intの場所に名前を入れても出来ます。
	//float f = Model->JointPosition(0, 0, &keyframe, world).x;
	//ImGui::Begin("DemoPlayer");// ここのnameをDemoPlayerに変えました
	//ImGui::DragFloat("PlayerPosition", &position.x);
	//ImGui::DragFloat("ScaleFactor", &scaleFactor);
	//ImGui::DragFloat("jointposition", &f);

	//ImGui::End();
}

void DemoPlayer::DrawDebugPrimitive()
{
	DebugRenderer* debugRenderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
	//衝突判定用のデバッグ円柱を描画
	debugRenderer->DrawCylinder(position, radius, height, DirectX::XMFLOAT4(0, 0, 0, 1));
}
