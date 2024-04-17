#include "GameScene.h"
#include "./Lemur/Component/DemoEnemyManager.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
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

		create_ps_from_cso(graphics.GetDevice(), "./Shader/differd_model_ps.cso", defefferd_model.GetAddressOf());
	}
	// ゲーム部分
	{
		// カメラ
		Camera& camera = Camera::Instance();
		// カメラ調整
		camera_range = 30.0f;

		// ステージ初期化
		StageManager& stage_manager = StageManager::Instance();
		StageMain* stage_main = new StageMain;
		stage_manager.Register(stage_main);

		// プレイヤーの初期化
		player = new Player;

		fence = new Fence;

		// 敵スポーン制御装置の初期化
		EnemySpawner::Instance().Initialize();

		ohajiki = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\おはじき.png");
	}

	// デバッグ
	{
		Try_T.B.y = 2.0;
		Try_T.C.x = 2.0;
		Try_T.C.y = 1.0;

		r0 = 0.1f;
	}
}

void GameScene::Finalize()
{
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

	// デバッグ用
	{
#if 0// 三角形当たり判定ためし
		DebugRenderer* debug_renderer = Lemur::Graphics::Graphics::Instance().GetDebugRenderer();

		debug_renderer->DrawSphere({ Try_T.A.x,1,Try_T.A.y }, 0.1f, { 1,0,1,1 });
		debug_renderer->DrawSphere({ Try_T.B.x,1,Try_T.B.y }, 0.1f, { 1,0,1,1 });
		debug_renderer->DrawSphere({ Try_T.C.x,1,Try_T.C.y }, 0.1f, { 1,0,1,1 });


		if (Collision::IntersectTriangleVsCircle(Try_T, P0, r0))
		{
			debug_renderer->DrawSphere({ P0.x,1,P0.y }, r0, { 1,0,0,1 });
		}
		else
		{
			debug_renderer->DrawSphere({ P0.x,1,P0.y }, r0, { 0,0,1,1 });
		}
#endif
		if (gamePad.GetButtonDown() & gamePad.BTN_A)
		{
			Enemy_D* enemy = new Enemy_D;
			// リストに追加
			EnemyManager::Instance().Register(enemy);
		}
	}

	// ゲーム
	{
		s_l = gamePad.GetAxisLY() * -1.0f;

		if (s_l > 0.1f)
		{
			timer_s += elapsedTime;
			if (s_l_max <= s_l)s_l_max = s_l;
		}
		else
		{
			if (timer_s > 0)f_d = (s_l_max * 100.0f) / timer_s;
			s_l_max = 0;
			timer_s = 0;
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
			player->Render(scale, defefferd_model.GetAddressOf());
			// 柵描画
			fence->Render(scale, defefferd_model.GetAddressOf());
			//ステージ描画
			StageManager::Instance().Render(scale, defefferd_model.GetAddressOf());
			// ユニット描画
			UnitManager::Instance().Render(scale, defefferd_model.GetAddressOf());
			// エネミー描画
			EnemyManager::Instance().Render(scale, defefferd_model.GetAddressOf());
			// 種描画
			SeedManager::Instance().Render(scale, defefferd_model.GetAddressOf());
		}
		else
		{
			// プレイヤー描画
			player->Render(scale, stage_ps.GetAddressOf());
			// 柵描画
			fence->Render(scale, stage_ps.GetAddressOf());
			//ステージ描画
			StageManager::Instance().Render(1.0f, stage_ps.GetAddressOf());
			// ユニット描画
			UnitManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// エネミー描画
			EnemyManager::Instance().Render(scale, chara_ps.GetAddressOf());
			// 種描画
			SeedManager::Instance().Render(scale, chara_ps.GetAddressOf());
		}
	}

	//TODO debug
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);
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
	BaseScene::DebugImgui();

	ImGui::Begin("ImGUI");
	ImGui::SliderFloat2("TA", &Try_T.A.x, -10.0f, 10.0f);
	ImGui::SliderFloat2("TB", &Try_T.B.x, -10.0f, 10.0f);
	ImGui::SliderFloat2("TC", &Try_T.C.x, -10.0f, 10.0f);
	ImGui::SliderFloat2("P0", &P0.x, -10.0f, 10.0f);
	ImGui::SliderFloat("r0", &r0, 0.0f, 10.0f);
	// STATIC_BATCHING
	if (ImGui::TreeNode("shadow"))
	{
		ImGui::Image(reinterpret_cast<void*>(double_speed_z->shader_resource_view.Get()), ImVec2(shadowmap_width / 5.0f, shadowmap_height / 5.0f));
		ImGui::SliderFloat("shadow_depth_bias", &scene_constants.shadow_depth_bias, 0.1f, 0.01f);
		ImGui::TreePop();
	}
	ImGui::End();
}
