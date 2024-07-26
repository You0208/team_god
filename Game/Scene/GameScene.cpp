#include "GameScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Scene/SceneManager.h"
// Game
#include "Game/CollisionManager.h"
#include "../Stage/StageManager.h"
#include "../Character/EnemyManager.h"
#include "../Character/Enemy_ABCD.h"
#include "../Character/UnitManager.h"
#include "../Character/Unit_ABC.h"
#include "../Character/Unit_DEF.h"
#include "../Character/SeedManager.h"
#include "../Character/EnemySpawner.h"
#include "../Scene/SelectScene.h"
#include "../Scene/ClearScene.h"
#include "../Scene/OverScene.h"
#include "../Scene/LoadingScene.h"
#include "../Scene/TitleScene.h"

#define _CRTDBG_MAP_ALLOC

void GameScene::Initialize()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// シェーダー関連
	{
		BaseScene::Initialize();

		// SHADOW
		shadow_map = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());

		// PS
		create_ps_from_cso(graphics.GetDevice(), "./Shader/sprite_ui.cso", sprite_ui.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_1.cso", stage_ps_1.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_2.cso", stage_ps_2.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/unit_ps.cso", unit_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/scarecrow.cso", scarecrow_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/enemy_ps.cso", enemy_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/collision_ps.cso", collision.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/collision_ps_2.cso", collision_2.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/fbx_gbuffer_ps.cso", fbx_gbuffer_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_gbuffer_ps.cso", gltf_gbuffer_ps.GetAddressOf());
	}
	// スプライト読み込み
	{
		if (Lemur::Scene::SceneManager::Instance().cont_type)	button_ui_base = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\button_UI.png");
		else 	button_ui_base = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\button_UI_PC.png");

		timer_hands         = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\timer_hands.png");
		timer_ui_base       = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\timer_base.png");
		button_ui_chara     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\UI_unit_sheet.png");
		button_ui_circle    = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\UI_unit_circle.png");

		start_text_1        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Game\\1.png");
		start_text_2        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Game\\2.png");
		start_text_3        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Game\\3.png");
		start_text_start    = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Game\\START.png");
		start_text_clear    = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Game\\CLEAR.png");
		fight_enemy         = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Game\\てきをたおそう.png");
		pose_exp            = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\pose_word.png");

		pause_main          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Pause_kakasi.png");
		pause_text_continue = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Continue.png");
		pause_text_select   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Stageselect.png");

		window              = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window.png");
		window_title        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_title.png");
		window_select       = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_stage.png");
		window_yes          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_yes.png");
		window_no           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\Window_no.png");

		stage_num[0]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\1-1.png");
		stage_num[1]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\1-2.png");
		stage_num[2]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\1-3.png");
		stage_num[3]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\2-1.png");
		stage_num[4]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\2-2.png");
		stage_num[5]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\2-3.png");
		stage_num[6]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\3-1.png");
		stage_num[7]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\3-2.png");
		stage_num[8]        = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\3-3.png");

		shadow_model        = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Enemy_Sphere.fbx");
	}
	// ゲーム部分
	{
		// カメラ
		Camera& camera = Camera::Instance();
		camera_range = 33.0f;
		camera_angle = { DirectX::XMConvertToRadians(rotation_camera_x.value),DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(0) };
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
		camera.SetEyeYOffset(8.0f);
		camera.SetAngle(camera_angle);

#if 0
		camera_angle = { DirectX::XMConvertToRadians(40.0f),DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(0) };
		camera.SetAngle(camera_angle);
#endif

		stage_num_text.value = -300;

		StageManager& stage_manager = StageManager::Instance();

		//0 1-1
		//1 1-2
		//2 1-3
		//3 2-1
		//4 2-2
		//5 2-3
		//6 3-1
		//7 3-2
		//8 3-3
		stage_manager.SetStageLevel(StageManager::Instance().GetStageLevel());

		//TODO もね 制限時間 ステージ選択
		switch (stage_manager.GetStageLevel())
		{
		case 0:// レベル１
			time_limit = 80.0f;
			break;
		case 1:// レベル２
			time_limit = 90.0f;
			break;
		case 2:// レベル３
			time_limit = 90.0f;
			break;
		case 3:// レベル４
			time_limit = 90.0f;
			break;
		case 4:// レベル５
			time_limit = 90.0f;
			break;
		case 5:// レベル6
			time_limit = 90.0f;
			break;
		case 6:// レベル7
			time_limit = 90.0f;
			break;
		case 7:// レベル8
			time_limit = 90.0f;
			break;
		case 8:// レベル9
			time_limit = 90.0f;
			break;
		}

		// タイマーの初期化
		timer = 0.0f;
		timer_angle = 360 * (timer / time_limit);
		
		// ステージ初期化
		stage_main = new StageMain;
		stage_manager.Register(stage_main);

		// プレイヤーの初期化
		player = new Player;
		fence = new Fence;

		// 敵スポーン制御装置の初期化
		EnemySpawner::Instance().Initialize();
		// ユニットマネージャーの初期化
		UnitManager::Instance().Initialize();
		// エネミーマネージャーの初期化
		EnemyManager::Instance().Initialize();

		// ポーズ画面
		is_pause = false;
		is_bloom = true;
		pause_text_continue_scale.value = 1.0f;
		pause_text_select_scale.value   = 1.0f;
		clear_direction_state           = 0;
		select_num = 1;
		// ワールドごとのライティング、色調補正
		// 朝
		if (stage_manager.GetStageLevel() == 0 ||
			stage_manager.GetStageLevel() == 1 ||
			stage_manager.GetStageLevel() == 2)
		{
			point_light[1].range           = 100;
			directional_light_direction    = { 0.737f,-1.00f,-0.795f,0.0f };
			option_constant.hsv_adjustment = { 1.0f,1.1f,1.7f,1.0f };
			option_constant.rgb_adjustment = { 1.0f,1.0f,1.15f,1.0f };
			option_constant.parameters.y   = 0.5f;

			option_constant.attack_color   = { 0.0f,1.0f,1.0f,0.11f };
			option_constant.edge_color     = { 0.0f,1.0f,1.0f,0.6f };

			option_constant.attack_color2  = { 1.0f,1.0f,0.0f,0.11f };
			option_constant.edge_color2    = { 1.0f,0.2f,0.0f,1.0f };

			// BGM
			Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_MORNING, true);
			Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_MORNING_SE, true);

		}
		// 昼
		if (stage_manager.GetStageLevel() == 3 ||
			stage_manager.GetStageLevel() == 4 ||
			stage_manager.GetStageLevel() == 5)
		{
			directional_light_direction    = { -0.342f,-1.00f,0.0f,0.0f };
			option_constant.hsv_adjustment = { 1.0f,1.1f,1.7f,1.0f };
			option_constant.rgb_adjustment = { 1.1f,1.0f,1.0f,1.0f };
			option_constant.parameters.y   = 0.5f;

			option_constant.attack_color   = { 0.0f,1.0f,1.0f,0.11f };
			option_constant.edge_color     = { 0.0f,1.0f,1.0f,0.6f };

			option_constant.attack_color2  = { 1.0f,1.0f,0.0f,0.11f };
			option_constant.edge_color2    = { 1.0f,0.2f,0.0f,1.0f };

			// BGM
			Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_NOON, true);
			Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_NOON_SE, true);

		}
		// 夜
		if (stage_manager.GetStageLevel() == 6 ||
			stage_manager.GetStageLevel() == 7 ||
			stage_manager.GetStageLevel() == 8)
		{
			point_light[0].position        = { 0.0f,3.0f,0.0f,1.0f };
			point_light[0].range           = 40;
			point_light[1].range           = 0;
			directional_light_direction    = { 0.111f,-1.00f,-0.676f,0.0f };
			option_constant.hsv_adjustment = { 1.0f,1.1f,1.0f,1.0f };
			option_constant.rgb_adjustment = { 1.0f,1.0f,1.3f,1.0f };
			option_constant.parameters.y   = 0.2f;

			option_constant.attack_color   = { 0.18f,1.0f,1.0f,0.07f };
			option_constant.edge_color     = { 0.3f,1.0f,1.0f,0.3f };

			option_constant.attack_color2  = { 1.0f,1.0f,0.0f,0.11f };
			option_constant.edge_color2    = { 1.0f,0.2f,0.0f,1.0f };

			// BGM
			Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_NIGHT, true);
			Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_NIGHT_SE, true);
		}

		// アイリスアウトを呼ぶ
		CallTransition(false);

		EnemyManager::Instance().SetTimeUp(false);
	}
}

void GameScene::Finalize()
{
	// BGM終了
	Lemur::Audio::AudioManager::Instance().StopAllBGM();
	Lemur::Audio::AudioManager::Instance().StopAllSE();

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
	// 敵スポーン制御装置の初期化
	EnemySpawner::Instance().Finalize();
	// ステージ終了
	StageManager::Instance().Clear();
}

void GameScene::Update(HWND hwnd, float elapsedTime)
{
	option_constant.parameters.y = 0.5f;

	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	// カメラ	
	camera.Update(elapsedTime);
	
	// ライトの更新
	LightUpdate();

	// マスクの更新
	TransitionMask(elapsedTime);

	// 遷移処理
	{
		if (preparation_next && !start_transition && is_in)
		{
			// 次のシーンへ
			if (next_scene == 0)Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new OverScene));
			if (next_scene == 1)Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new ClearScene));
		}
		// 遷移演出の最中はこれより下に行かない
		if (start_transition)return;
		if (preparation_next)return;
	}
	// 演出
#if 1
	if(start_direction)
	{
		StartDirectionUpdate(elapsedTime);
		return;
	}
	else if (over_direction)
	{
		fence->CallFenceShake();
		OverDirectionUpdate(elapsedTime);
		return;
	}
	else if (clear_direction)
	{
		StageManager::Instance().result_health_parsent = fence->health_prsent;
		ClearDirectionUpdate(elapsedTime);
		return;
	}
#endif
	// ゲームクリア、オーバーの判定
	{
		// タイムアップかつ敵が全て死んだら
		if (EnemyManager::Instance().NonEnemy() && time_up)
		{
			clear_direction = true;
		}
		else if (fence->GetHealth() <= 0)
		{
			fence->fence_state = fence->FANCE_2;
			over_direction = true;
		}
	}
	// ポーズ画面
	{
		if (is_pause)
		{
			PauseUpdate(elapsedTime);
			return;
		}
		else is_bloom = true;
		if (gamePad.GetButtonDown() & gamePad.BTN_START || GetAsyncKeyState(VK_BACK) & 0x0001)
		{
			pause_window_scale.value = 0.0f;
			is_pause = !is_pause;
			pause_text_continue_scale.CallValueContinue(1.0f, 1.1f, pause_text_continue_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
		}
	}
	// ゲーム
#if 1
	{
		if (timer >= time_limit)
		{
			EnemyManager::Instance().SetTimeUp(true);
			time_up = true;
		}
		if (time_up)
		{
			EnemyManager::Instance().PowerUpEnemy();
		}
		else
		{
			timer_angle = 360 * (timer / time_limit);
			timer += elapsedTime;
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
#else
		//ステージ更新処理
	StageManager::Instance().update(elapsedTime);
#endif

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
	SetUpConstantBuffer();

	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	// スケール
	const float scale = 0.015f;
	// TODO もね　
	const float enemy_scale = 0.016f;

	// 影
	{
		SetUpShadowMap();

		shadow_map->Clear(immediate_context);
		shadow_map->Activate(immediate_context);

		ID3D11PixelShader* null_pixel_shader{ NULL };

		// ●影
		for (int i=0;i<EnemyManager::Instance().GetEnemyCount();i++)
		{
			shadow_model->GetTransform()->SetPosition({ EnemyManager::Instance().GetEnemy(i)->GetPosition().x,-0.3f, EnemyManager::Instance().GetEnemy(i)->GetPosition().z });
			shadow_model->Render(scale * 13.0f, &null_pixel_shader);
		}

		// プレイヤー描画
		player->Render(scale, &null_pixel_shader);

		//// ステージ描画
		StageManager::Instance().Render(1.0f, &null_pixel_shader, &null_pixel_shader);
		// ●影
		for (int i = 0; i < UnitManager::Instance().GetUnitCount(); i++)
		{
			shadow_model->GetTransform()->SetPosition({ UnitManager::Instance().GetUnit(i)->GetPosition().x,-0.3f, UnitManager::Instance().GetUnit(i)->GetPosition().z });
			shadow_model->Render(scale * 13.0f, &null_pixel_shader);
		}
		// 種描画
		SeedManager::Instance().Render(0.1f, null_pixel_shader);

		shadow_map->Deactivate(immediate_context);
	}

	// 定数バッファーの更新
	SetUpConstantBuffer();

	// エッジ検出
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Activate(immediate_context);
		ID3D11PixelShader* null_pixel_shader{ NULL };
		player->Render(0.025f, &null_pixel_shader);
		UnitManager::Instance().CollisionRender(scale, null_pixel_shader, null_pixel_shader);
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
		immediate_context->PSSetShaderResources(9/*slot(1番にセットします)*/, 1, noise.GetAddressOf());
		// シャドウ
		immediate_context->PSSetShaderResources(8, 1, shadow_map->shader_resource_view.GetAddressOf());
		//　深度値
		immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
		// PROJECTION_MAPPING
		//immediate_context->PSSetShaderResources(15, 1, projection_mapping_texture.GetAddressOf());
	}
	// ポストエフェクトの開始
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context,0.2f,0.3f,0.5f,1.0f);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
	}

	//3D描画
	{
		if (enable_deferred)
		{
			// プレイヤー描画
			player->Render(scale, fbx_gbuffer_ps.GetAddressOf());
			// 柵描画
			fence->Render(scale, fbx_gbuffer_ps.GetAddressOf());
			//ステージ描画
			//StageManager::Instance().Render(1.0f, fbx_gbuffer_ps.GetAddressOf());
			// ユニット描画
			UnitManager::Instance().Render(scale, fbx_gbuffer_ps.Get());
			// エネミー描画
			EnemyManager::Instance().Render(enemy_scale, fbx_gbuffer_ps.Get());
			// 種描画
			SeedManager::Instance().Render(scale, fbx_gbuffer_ps.Get());
		}
		else
		{
			// プレイヤー描画
			player->Render(0.025f, scarecrow_ps.GetAddressOf());

			// 柵描画
			//fence->Render(scale, Try.GetAddressOf());

			// ステージ描画
			StageManager::Instance().Render(1.0f, stage_ps_1.GetAddressOf(), stage_ps_2.GetAddressOf());
			// ユニット描画
			UnitManager::Instance().Render(scale, unit_ps.Get());
			UnitManager::Instance().CollisionRender(scale, collision.Get(), collision_2.Get());
			// エネミー描画
			EnemyManager::Instance().Render(enemy_scale, enemy_ps.Get());
			// 種描画
			SeedManager::Instance().Render(0.1f, enemy_ps.Get());
		}
	}

	// ステートの再設定
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());

	// バッファーの変更
	RenderingDeffered();

	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);
		// エフェクト再生
		EffectManager::Instance().Render(view, projection);
		// ブレンドステート設定
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステート設定
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 0);
		// ラスタライザーステート設定
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	}
	// ポストエフェクトの実行
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}

	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

	if (is_bloom && !is_pause)
	{
		GamePad& gamePad = Input::Instance().GetGamePad();

		if (!clear_direction && !over_direction && !start_direction && !Lemur::Scene::SceneManager::Instance().cont_type)
		{
			player->GaugeRender();
		}
		// 2D描画
		button_ui_base->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A], SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B], SCREEN_HEIGHT * 1, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X], SCREEN_HEIGHT * 2, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y], SCREEN_HEIGHT * 3, SCREEN_WIDTH, SCREEN_HEIGHT);

		if(Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A]==player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * 0, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		else if(Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B]==player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH *1, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		else if(Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X]==player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH *2, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		else if(Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y]==player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * 3, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		timer_ui_base->Render(immediate_context, sprite_ui.Get(), 50, 50, 214, 273, 0.0f);
		timer_hands->RenderCenter(immediate_context, sprite_ui.Get(), 50 + 107, 50 + 167, 4, 180, timer_angle);

		if (start_direction)
		{
			if (start_direction_state == START_DIRECTION::Three || start_direction_state == START_DIRECTION::Three_End)
			{
				start_text_3->RenderCenter(immediate_context, 960.0f, 540.0f, SCREEN_WIDTH * scale_start_direction.value, SCREEN_HEIGHT * scale_start_direction.value);
			}
			else if (start_direction_state == START_DIRECTION::Two || start_direction_state == START_DIRECTION::Two_End)
			{
				start_text_2->RenderCenter(immediate_context, 960.0f, 540.0f, SCREEN_WIDTH * scale_start_direction.value, SCREEN_HEIGHT * scale_start_direction.value);
			}
			else if (start_direction_state == START_DIRECTION::One || start_direction_state == START_DIRECTION::One_End)
			{
				start_text_1->RenderCenter(immediate_context, 960.0f, 540.0f, SCREEN_WIDTH * scale_start_direction.value, SCREEN_HEIGHT * scale_start_direction.value);
			}
			else if (start_direction_state == START_DIRECTION::START || start_direction_state == START_DIRECTION::START_End)
			{
				start_text_start->RenderCenter(immediate_context, 960.0f, 540.0f, SCREEN_WIDTH * scale_start_direction.value, SCREEN_HEIGHT * scale_start_direction.value);
			}

			stage_num[StageManager::Instance().GetStageLevel()]->RenderCenter(immediate_context, stage_num_text.value, 540, 600, 450);
		}

	}
	else if (!is_bloom && !is_pause)
	{
		if (clear_direction)
		{
			start_text_clear->RenderCenter(immediate_context, 960.0f, 540.0f, SCREEN_WIDTH * clear_text_scale.value, SCREEN_HEIGHT * clear_text_scale.value);
		}
	}
	else if(!is_bloom && is_pause)
	{
		pause_main->Render(immediate_context, SCREEN_WIDTH - 820, 0, 820, SCREEN_HEIGHT);
		pause_text_continue->RenderCenter(immediate_context, 1520, 550, 500 * pause_text_continue_scale.value, 120 * pause_text_continue_scale.value);
		pause_text_select->RenderCenter(immediate_context, 1520, 700, 500 * pause_text_select_scale.value, 120 * pause_text_select_scale.value);
		pose_exp->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

	}
	if (timer >= time_limit)fight_enemy->RenderCenter(immediate_context, SCREEN_WIDTH * 0.5f, 70, 700, 120);

	if (is_pause)
	{
		window->RenderCenter(immediate_context, 960, 540, 1400 * pause_window_scale.value, 700 * pause_window_scale.value);
		if (is_select_text)window_select->RenderCenter(immediate_context, 960, 420, 1100 * pause_window_scale.value, 200 * pause_window_scale.value);
		if (is_title_text)window_title->RenderCenter(immediate_context, 960, 420, 1100 * pause_window_scale.value, 200 * pause_window_scale.value);
		window_yes->RenderCenter(immediate_context, 700, 645, 200 * pause_window_scale.value * yes.value, 100 * pause_window_scale.value * yes.value);
		window_no->RenderCenter(immediate_context, 1200, 645, 200 * pause_window_scale.value * no.value, 100 * pause_window_scale.value * no.value);
	}
	// マスクの描画
	RenderTransitionMask(elapsedTime);
}

void GameScene::DebugImgui()
{
#ifdef  DEBUG_IMGUI
	ImGui::Begin(u8"ゲーム");
	float t = time_limit - timer;
	ImGui::DragFloat(u8"残り時間", &t, 0.0f, 0.1f);
	ImGui::SliderFloat(u8"制限時間", &time_limit, 0.0f, 600.0f);
	ImGui::End();

	ImGui::Begin("shadow_color");
	ImGui::SliderFloat3("shadow_color", &scene_constant.shadow_color.x, 0.0f, 1.0f);
	ImGui::End();

	BaseScene::DebugImgui();
	Camera::Instance().DrawDebug();
	EnemySpawner::Instance().DebugImGui();
	player->DrawDebugGUI();
#endif
}

void GameScene::InitializeLight()
{
	point_light[0].position.x = 0;
	point_light[0].position.y = 50;
	point_light[0].range = 1000;
	point_light[0].color = { 1, 1, 1, 1 };
	point_light[1].position.x = 0;
	point_light[1].position.y = 10;
	point_light[1].range = 1000;
	point_light[1].color = { 1, 1, 1, 1 };
	ZeroMemory(&point_light[8], sizeof(pointLights) * 8);
	ZeroMemory(&spot_light[4], sizeof(spotLights) * 4);
}

void GameScene::PauseUpdate(float elapsedTime)
{
	is_bloom = false;
	GamePad& gamePad = Input::Instance().GetGamePad();
	Mouse& mouse = Input::Instance().GetMouse();

	bool down_pressed = gamePad.GetAxisLY() <= -0.5f || gamePad.GetAxisRY() <= -0.5f || (gamePad.GetButtonDown() & gamePad.BTN_DOWN) || GetAsyncKeyState(VK_DOWN) & 0x8000 || GetAsyncKeyState('S') & 0x8000;
	bool up_pressed = gamePad.GetAxisLY() >= 0.5f || gamePad.GetAxisRY() >= 0.5f || (gamePad.GetButtonDown() & gamePad.BTN_UP) || GetAsyncKeyState(VK_UP) & 0x8000 || GetAsyncKeyState('W') & 0x8000;
	bool buttonB_pressed = gamePad.GetButtonDown() & gamePad.BTN_B || mouse.GetButtonDown() & mouse.BTN_LEFT;

	if (GetAsyncKeyState(VK_RETURN)& 0x8000)buttonB_pressed = true;

	float f = static_cast<float>(mouse.GetWheel());
	if (mouse.GetWheel() != 0 && f < 0)
	{
		down_pressed = true;
	}
	if (mouse.GetWheel() != 0 && f > 0)
	{
		up_pressed = true;
	}

	if (!is_select_text)
	{
		pause_window_scale.EasingValue(elapsedTime);
		if (pause_window_scale.is_easing)return;

		if (gamePad.GetButtonDown() & gamePad.BTN_START || GetAsyncKeyState(VK_BACK) & 0x0001)
		{
			is_title_text = true;
			pause_window_scale.CallValueEasing(1.0f, pause_window_scale.value, pause_window_scale.OutSine);
			no.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::OutSine);
			yes.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::OutSine);
			no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
		}
		if (is_title_text)
		{

			switch (select_num)
			{
			case Button::YES:
				// 右を選んだとき
				if (gamePad.GetAxisLX() >= 0.5f || gamePad.GetAxisRX() >= 0.5f || gamePad.GetButtonDown() & gamePad.BTN_RIGHT || GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
				{
					Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
					no.CallValueEasing(1.1f, no.value, EasingFunction::EasingType::InSine);
					yes.CallValueEasing(1.0f, yes.value, EasingFunction::EasingType::InSine);
					no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
					select_num = Button::NO;
					break;
				}
				// イージングを更新
				yes.EasingValue(elapsedTime);
				no.EasingValue(elapsedTime);
				yes.ContinueEasing(elapsedTime);

				// 選択されると
				if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
				{
					Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
					// アイリスインを呼ぶ
					CallTransition(true);
				}

				if (!start_transition && is_in)
				{
					is_bloom = true;
					// 次のシーンへ
					Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new TitleScene));
				}
				break;

			case Button::NO:
				if (gamePad.GetAxisLX() <= -0.5f || gamePad.GetAxisRX() <= -0.5f || gamePad.GetButtonDown() & gamePad.BTN_LEFT || GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)// 左選択すると
				{
					Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
					yes.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::InSine);
					no.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::InSine);
					yes.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
					select_num = Button::YES;
					break;
				}

				// イージングを更新
				no.EasingValue(elapsedTime);
				yes.EasingValue(elapsedTime);
				no.ContinueEasing(elapsedTime);

				// 選択されると
				if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
				{
					is_title_text = false;
					select_num = 1;
					pause_window_scale.CallValueEasing(0.0f, pause_window_scale.value, pause_window_scale.OutSine);
				}
				break;
			}
			return;
		}
	}

	switch (pause_num)
	{
	case PAUSE_STATE::CONTINUE:
		pause_text_continue_scale.ContinueEasing(elapsedTime);
		pause_text_select_scale.EasingValue(elapsedTime);
		if (down_pressed)
		{
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
			pause_text_select_scale.CallValueContinue(1.0f, 1.1f, pause_text_select_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
			pause_text_continue_scale.CallValueEasing(1.0f, pause_text_continue_scale.value, EasingFunction::EasingType::InSine);
			pause_num = PAUSE_STATE::SELECT;
		}
		else if(buttonB_pressed)
		{
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
			is_bloom = true;
			pause_num = 0;
			is_pause = false;
		}
		break;
	case PAUSE_STATE::SELECT:
		pause_window_scale.EasingValue(elapsedTime);
		if (pause_window_scale.is_easing)return;

		pause_text_select_scale.ContinueEasing(elapsedTime);
		pause_text_continue_scale.EasingValue(elapsedTime);
		if (up_pressed)
		{
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
			pause_text_continue_scale.CallValueContinue(1.0f, 1.1f, pause_text_continue_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
			pause_text_select_scale.CallValueEasing(1.0f, pause_text_select_scale.value, EasingFunction::EasingType::InSine);
			pause_num= PAUSE_STATE::CONTINUE;
		}
		else if (buttonB_pressed)
		{
			is_select_text = true;
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
			pause_num = PAUSE_STATE::SELECT_END;
			pause_window_scale.CallValueEasing(1.0f, pause_window_scale.value, pause_window_scale.OutSine);
			no.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::OutSine);
			yes.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::OutSine);
			no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
		}
		break;
	case PAUSE_STATE::SELECT_END:
		pause_window_scale.EasingValue(elapsedTime);
		if (pause_window_scale.is_easing)return;

		switch (select_num)
		{
		case Button::YES:
			// 右を選んだとき
			if (gamePad.GetAxisLX() >= 0.5f || gamePad.GetAxisRX() >= 0.5f || gamePad.GetButtonDown() & gamePad.BTN_RIGHT || GetAsyncKeyState(VK_RIGHT) & 0x8000 || GetAsyncKeyState('D') & 0x8000)
			{
				Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
				no.CallValueEasing(1.1f, no.value, EasingFunction::EasingType::InSine);
				yes.CallValueEasing(1.0f, yes.value, EasingFunction::EasingType::InSine);
				no.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
				select_num = Button::NO;
				break;
			}
			// イージングを更新
			yes.EasingValue(elapsedTime);
			no.EasingValue(elapsedTime);
			yes.ContinueEasing(elapsedTime);

			// 選択されると
			if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::DECISION, false);
				CallTransition(true);
			}

			if (!start_transition && is_in)
			{
				is_bloom = true;
				// 次のシーンへ
				Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
			}
			break;

		case Button::NO:
			if (gamePad.GetAxisLX() <= -0.5f || gamePad.GetAxisRX() <= -0.5f || gamePad.GetButtonDown() & gamePad.BTN_LEFT || GetAsyncKeyState(VK_LEFT) & 0x8000 || GetAsyncKeyState('A') & 0x8000)// 左選択すると
			{
				Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::STICK, false);
				yes.CallValueEasing(1.1f, yes.value, EasingFunction::EasingType::InSine);
				no.CallValueEasing(1.0f, no.value, EasingFunction::EasingType::InSine);
				yes.CallValueContinue(1.0f, 1.1f, 1.1f, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine, 0.4f);
				select_num = Button::YES;
				break;
			}

			// イージングを更新
			no.EasingValue(elapsedTime);
			yes.EasingValue(elapsedTime);
			no.ContinueEasing(elapsedTime);

			// 選択されると
			if (gamePad.GetButtonDown() & gamePad.BTN_B || GetAsyncKeyState(VK_RETURN) & 0x8000 || mouse.GetButtonDown() & mouse.BTN_LEFT)
			{
				is_select_text = false;
				select_num = 1;
				pause_window_scale.CallValueEasing(0.0f, pause_window_scale.value, pause_window_scale.OutSine);
				pause_num = PAUSE_STATE::SELECT;
			}
			break;
		}

		break;
	}
}

void GameScene::StartDirectionUpdate(float elapsedTime)
{
	Camera& camera = Camera::Instance();

	scale_start_direction.EasingValue(elapsedTime);

	stage_num_text.EasingValue(elapsedTime);
	switch (start_direction_state)
	{
	case START_DIRECTION::Stage_num:
		camera_angle = { DirectX::XMConvertToRadians(rotation_camera_x.value),DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(0) };
		camera.SetAngle(camera_angle);

		stage_num_text.CallValueEasing(960, stage_num_text.value, stage_num_text.OutSine, 0.5f);
		start_direction_state = START_DIRECTION::Stage_num_end;
		break;
	case START_DIRECTION::Stage_num_end:
		if (stage_num_text.value >= 960 && stage_num_text.value < 2220 && !stage_num_text.is_easing)
		{
			stage_num_text.CallValueEasing(2220, stage_num_text.value, stage_num_text.InSine, 0.5f);
		}
		if (stage_num_text.value >= 1920 && !stage_num_text.is_easing)
		{
			// イージングを呼ぶ
			rotation_camera_x.CallValueEasing(40.0f, rotation_camera_x.value, EasingFunction::EasingType::OutSine, 1.0f);
			start_direction_state = START_DIRECTION::Camera_direction;
		}
		break;
	case START_DIRECTION::Camera_direction:// カメラ移動
		camera_angle = { DirectX::XMConvertToRadians(rotation_camera_x.value),DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(0) };
		camera.SetAngle(camera_angle);
		rotation_camera_x.EasingValue(elapsedTime);

		if (!rotation_camera_x.is_easing)
		{
			scale_start_direction.CallValueEasing(1.0f, scale_start_direction.value, EasingFunction::EasingType::OutSine, start_in_time);
			start_direction_state = START_DIRECTION::Three;
		}
		break;
	case START_DIRECTION::Three:// 文字３
		if (!scale_start_direction.is_easing)
		{
			scale_start_direction.CallValueEasing(0.0f, scale_start_direction.value, EasingFunction::EasingType::InSine, start_out_time);
			start_direction_state = START_DIRECTION::Three_End;
		}

		break;
	case START_DIRECTION::Three_End:// 文字３
		if (!scale_start_direction.is_easing)
		{
			scale_start_direction.CallValueEasing(1.0f, scale_start_direction.value, EasingFunction::EasingType::OutSine, start_in_time);
			start_direction_state = START_DIRECTION::Two;
		}

		break;
	case START_DIRECTION::Two:// 文字３
		if (!scale_start_direction.is_easing)
		{
			scale_start_direction.CallValueEasing(0.0f, scale_start_direction.value, EasingFunction::EasingType::InSine, start_out_time);
			start_direction_state = START_DIRECTION::Two_End;
		}
		break;
	case START_DIRECTION::Two_End:// 文字３
		if (!scale_start_direction.is_easing)
		{
			scale_start_direction.CallValueEasing(1.0f, scale_start_direction.value, EasingFunction::EasingType::OutSine, start_in_time);
			start_direction_state = START_DIRECTION::One;
		}

		break;
	case START_DIRECTION::One:// 文字３
		if (!scale_start_direction.is_easing)
		{
			scale_start_direction.CallValueEasing(0.0f, scale_start_direction.value, EasingFunction::EasingType::InSine, start_out_time);
			start_direction_state = START_DIRECTION::One_End;
		}
		break;
	case START_DIRECTION::One_End:// 文字３
		if (!scale_start_direction.is_easing)
		{
			scale_start_direction.CallValueEasing(1.0f, scale_start_direction.value, EasingFunction::EasingType::InSine, 1.0f);
			start_direction_state = START_DIRECTION::START;
		}
		break;
	case START_DIRECTION::START:// 文字３
		if (!scale_start_direction.is_easing)
		{
			start_direction_state = START_DIRECTION::START_End;
		}
		break;
	case  START_DIRECTION::START_End:
		start_timer += elapsedTime;
		if (start_timer >= 0.5f)start_direction = false;
		break;
	}

}

void GameScene::OverDirectionUpdate(float elapsedTime)
{
	over_timer += elapsedTime;

	switch (over_state)
	{
	case 0:
		fence->FenceShake(elapsedTime);

		if (over_timer >= 1.0f)
		{
			fence->StopFenceShake();
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::FANCE_BREAK, false);

			fence->fence_state = fence->FANCE_2;
			over_timer = 0.0f;
			over_state++;
		}

		break;
	case 1:
		if (over_timer >= 0.5f)
		{
			next_scene = 0;
			preparation_next = true;
			CallTransition(true);
		}
		break;
	}
}

void GameScene::ClearDirectionUpdate(float elapsedTime)
{
	StageManager& stage_manager = StageManager::Instance();

	clear_text_scale.EasingValue(elapsedTime);
	switch (clear_direction_state)
	{
	case 0:
		is_bloom = false;
		if (stage_manager.GetStageLevel() <= 2)
		{
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::CHICKEN, false);
		}
		else if (stage_manager.GetStageLevel() <= 5)
		{
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::BARD, false);
		}
		else if (stage_manager.GetStageLevel() <= 8)
		{
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::OWL, false);
		}
		clear_direction_state++;
		break;
	case 1:
		clear_timer += elapsedTime;

		if (clear_timer >= 1.0f)
		{
			Lemur::Audio::AudioManager::Instance().PlaySe(Lemur::Audio::SE::CLEAR, false);
			clear_text_scale.CallValueEasing(1.0f, clear_text_scale.value, EasingFunction::EasingType::OutBounce, 1.0f);
			clear_timer = 0;
			clear_direction_state++;
		}
		break;
	case 2:
		clear_timer += elapsedTime;

		if (clear_timer >= 1.0f)
		{
			clear_direction_state++;
		}
		break;
	case 3:
		if (!clear_text_scale.is_easing)
		{
			is_bloom = false;
			next_scene = 1;
			preparation_next = true;
			CallTransition(true);
		}
		break;

	}
}
