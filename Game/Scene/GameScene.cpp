#include "GameScene.h"
#include "./Lemur/Component/DemoEnemyManager.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "Game/Scene/ResultScene.h"
#include "./high_resolution_timer.h"

// Effect
#include "./Lemur/Effekseer/EffekseerManager.h"

// Game
#include "Game/CollisionManager.h"
#include "../Stage/StageMain.h"
#include "../Stage/StageManager.h"
#include "../Character/EnemyManager.h"
#include "../Character/Enemy_ABCD.h"
#include "../Character/UnitManager.h"
#include "../Character/Unit_ABC.h"
#include "../Character/Unit_DEF.h"
#include "../Character/SeedManager.h"
#include "../Character/EnemySpawner.h"

#include "Lemur/Graphics/DebugRenderer.h"

void GameScene::Initialize()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// シェーダー関連
	{
		// ステートの初期化
		InitializeState();
		// ディファードレンダリングの初期化
		InitializeDeffered(SCREEN_WIDTH, SCREEN_HEIGHT);
		// フレームバッファーの初期化
		InitializeFramebuffer();
		// ピクセルシェーダーの初期化
		InitializePS();
		// ポイントライト・スポットライトの初期位置設定
		InitializeLight();

		// スカイマップテクスチャのロード
		load_texture_from_file(graphics.GetDevice(), L".\\resources_2\\winter_evening_4k.hdr", skymap.GetAddressOf(), graphics.GetTexture2D());

		// SHADOW
		double_speed_z = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		load_texture_from_file(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());//TODO

		//TODO 実験用
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", Try.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", chara_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps.cso", stage_ps.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/fbx_gbuffer_ps.cso", fbx_gbuffer_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_gbuffer_ps.cso", gltf_gbuffer_ps.GetAddressOf());
	}
	// ゲーム部分
	{
		// カメラ
		Camera& camera = Camera::Instance();
		// カメラ調整
		camera_range = 30.0f;


		StageManager& stage_manager = StageManager::Instance();
		stage_manager.SetStageLevel(1);

		//TODO もね 制限時間の初期化、ステージ
		switch (StageManager::Instance().GetStageLevel())
		{
		case 0:// レベル１
			time_limit = 80.0f;// ここで制限時間を調整
			break;
		case 1:// レベル２
			time_limit = 80.0f;// ここで制限時間を調整
			break;
		case 2:// レベル３
			time_limit = 200.0f;// ここで制限時間を調整
			break;
		case 3:// レベル４
			time_limit = 200.0f;// ここで制限時間を調整
			break;
		case 4:// レベル５
			time_limit = 200.0f;// ここで制限時間を調整
			break;
		}
		timer = 0.0f;
		
		// ステージ初期化
		StageMain* stage_main = new StageMain;
		stage_manager.Register(stage_main);

		// プレイヤーの初期化
		player = new Player;

		fence = new Fence;

		// 敵スポーン制御装置の初期化
		EnemySpawner::Instance().Initialize();
		// ユニットマネージャーの初期化
		UnitManager::Instance().Initialize();

		ohajiki = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\おはじき.png");
	}

	// デバッグ
	{
		Try_T.B.y = 2.0;
		Try_T.C.x = 2.0;
		Try_T.C.y = 1.0;
		angle = 50.0f;
		r0 = 0.1f;

		test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\Jummo\\Jummo.fbx");
		test_model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\grid.fbx");

		//rect.center = { 0,0 };
		//rect = CalcRotateRect(rect.center, { 2,2 }, angle);
		//rect.width = { 2,2 };
		//c_p = { 0.0f,0.0f };
		//c_r = 0.5f;
		//hitEffect = new Effect(".\\resources\\Effect\\Unit_set.efk");
	}
}

void GameScene::Finalize()
{
	// デバッグ
	// エフェクト終了
	if (hitEffect != nullptr)
	{
		delete hitEffect;
		hitEffect = nullptr;
	}

	//ステージ終了
	if (stage != nullptr)
	{
		delete stage;
		stage = nullptr;
	}
	//プレイヤー終了
	if (player != nullptr)
	{
		delete player;
		player = nullptr;
	}
	//プレイヤー終了
	if (fence != nullptr)
	{
		delete fence;
		fence = nullptr;
	}
	// エネミー終了
	EnemyManager::Instance().Clear();
	// ユニット終了
	UnitManager::Instance().Clear();
	// 種終了
	SeedManager::Instance().Clear();
}

void GameScene::Update(HWND hwnd, float elapsedTime)
{
	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();

	// カメラ
	{
		camera.Update(elapsedTime);
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
	}

	// ライトの更新
	LightUpdate();

	// ゲーム
	{
		// タイマー
		timer += elapsedTime;

		if (timer >= time_limit)
		{
			Lemur::Scene::SceneManager::Instance().ChangeScene(new ResultScene);
		}


		// プレイヤーの更新
		player->Update(elapsedTime);
		// 柵の更新
		fence->Update(elapsedTime);
		// エフェクト更新処理
		EffectManager::Instance().Update(elapsedTime);
		//ステージ更新処理
		StageManager::Instance().update(elapsedTime);
		EnemyManager::Instance().Update(elapsedTime);
		UnitManager::Instance().Update(elapsedTime);
		// 種更新
		SeedManager::Instance().Update(elapsedTime);

		// 種とユニットの当たり判定（種が生まれるか）
		CollisionManager::Instance().CollisionSeedVsUnit();

		// スポーン
		EnemySpawner::Instance().Update(elapsedTime);
	}

	// デバッグ
	{
		//rect = CalcRotateRect(rect.center, rect.width, angle);
	}

	// Imgui
	DebugImgui();
}

void GameScene::Render(float elapsedTime)
{
	HRESULT hr{ S_OK };

	Camera& camera = Camera::Instance();
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();
	ID3D11DeviceContext* immediate_context = graphics.GetDeviceContext();
	ID3D11RenderTargetView* render_target_view = graphics.GetRenderTargetView();
	ID3D11DepthStencilView* depth_stencil_view = graphics.GetDepthStencilView();

	camera.SetPerspectiveFov(immediate_context);
	
	// 描画の設定
	SetUpRendering();

	// エッジ検出
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Activate(immediate_context);
		ID3D11PixelShader* null_pixel_shader{ NULL };
		//EnemyManager::Instance().Render(0.1f, null_pixel_shader);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Deactivate(immediate_context);
	}

	// ディファードレンダリングの設定
	SetUpDeffered();

	//ステートの設定
	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::MLT_ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	// テクスチャをセット
	{
		// ノイズ
		immediate_context->PSSetShaderResources(9/*slot(1番にセットします)*/, 1, noise.GetAddressOf());//TODO
		// シャドウ
		immediate_context->PSSetShaderResources(8, 1, double_speed_z->shader_resource_view.GetAddressOf());
		//　深度値
		immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
	}

	// ポストエフェクトの開始
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
	}

	//3D描画
	{
		const float scale = 0.01f;
		if (enable_deferred)
		{
			// プレイヤー描画
			player->Render(scale, fbx_gbuffer_ps.GetAddressOf());
			// 柵描画
			fence->Render(scale, fbx_gbuffer_ps.GetAddressOf());
			//ステージ描画
			StageManager::Instance().Render(1.0f, fbx_gbuffer_ps.GetAddressOf());
			// ユニット描画
			UnitManager::Instance().Render(scale, fbx_gbuffer_ps.GetAddressOf());
			// エネミー描画
			EnemyManager::Instance().Render(scale, fbx_gbuffer_ps.GetAddressOf());
			// 種描画
			SeedManager::Instance().Render(scale, fbx_gbuffer_ps.GetAddressOf());
		}
		else
		{
			// プレイヤー描画
			player->Render(scale, chara_ps.GetAddressOf());
			// 柵描画
			fence->Render(scale, chara_ps.GetAddressOf());
			//ステージ描画
			StageManager::Instance().Render(1.0f, chara_ps.GetAddressOf());
			// ユニット描画
			UnitManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// エネミー描画
			EnemyManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// 種描画
			SeedManager::Instance().Render(scale, chara_ps.GetAddressOf());
			//test_model->Render(0.01f, Try.Get());
			//test_model_2->Render(0.1f, Try.Get());
			//test_model->DrawDebug("Test");
			//test_model_2->DrawDebug("Test");
		}
	}

	//TODO debug
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);
		EffectManager::Instance().Render(view, projection);

		//DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();
		//debug_renderer->DrawBox({ rect.center.x,0,rect.center.y }, { 0,DirectX::XMConvertToRadians(angle),0 }, { rect.width.x * 0.5f,0.5f,rect.width.y * 0.5f }, { 1,0,1,1 });
		//debug_renderer->DrawSphere({ rect.right_down.x,0.5f, rect.right_down.y }, 0.1f, { 0,0,1,1 });
		//debug_renderer->DrawSphere({ rect.left_up.x,0.5f, rect.left_up.y }, 0.1f, { 0,1,0,1 });

		//if (Collision::IntersectRotateRectVsCircle(rect, c_p, c_r,angle))debug_renderer->DrawSphere({ c_p.x,0.5f, c_p.y }, c_r, { 1,1,0,1 });
		//else debug_renderer->DrawSphere({ c_p.x,0.5f, c_p.y }, c_r, { 0,0,1,1 });
	}

	// ステートの再設定
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

	// バッファーの変更
	RenderingDeffered();

	// ポストエフェクトの実行
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}
}

void GameScene::DebugImgui()
{
	ImGui::Begin(u8"ゲーム");
	float t = time_limit - timer;
	ImGui::DragFloat(u8"残り時間", &t, 0.0f, 0.1f);
	ImGui::SliderFloat(u8"制限時間", &time_limit, 0.0f, 600.0f);
	ImGui::End();

	ImGui::Begin("debug");
	ImGui::SliderFloat2("rect_center", &rect.center.x, -10.0f, 10.0f);
	ImGui::SliderFloat2("rect_width", &rect.width.x, 0.0f, 10.0f);
	ImGui::SliderFloat("angle", &angle, 0.0f, 360.0f);
	ImGui::SliderFloat2("c_p", &c_p.x, -10.0f, 10.0f);
	ImGui::SliderFloat("c_r", &c_r, -10.0f, 10.0f);
	ImGui::End();

	BaseScene::DebugImgui();
	Camera::Instance().DrawDebug();

}
