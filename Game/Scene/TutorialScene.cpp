#include "TutorialScene.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Scene/SceneManager.h"

// Game
#include "SelectScene.h"
#include "LoadingScene.h"
#include "../CollisionManager.h"
#include "../Stage/StageMain.h"
#include "../Stage/StageManager.h"
#include "../Character/EnemyManager.h"
#include "../Character/Enemy_ABCD.h"
#include "../Character/UnitManager.h"
#include "../Character/Unit_ABC.h"
#include "../Character/Unit_DEF.h"
#include "../Character/SeedManager.h"
#include "../Character/EnemySpawner.h"
#include "../Character/UniqueEnemy.h"


void TutorialScene::Initialize()
{
	Lemur::Graphics::Graphics& graphics = Lemur::Graphics::Graphics::Instance();

	// シェーダー関連
	{
		BaseScene::Initialize();

		// SHADOW
		shadow_map = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());

		//PS
		create_ps_from_cso(graphics.GetDevice(), "./Shader/sprite_ui.cso", sprite_ui.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_1.cso", stage_ps_1.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_2.cso", stage_ps_2.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/unit_ps.cso", unit_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/scarecrow.cso", scarecrow_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/enemy_ps.cso", enemy_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/collision_ps.cso", collision.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/collision_ps_2.cso", collision_2.GetAddressOf());
	}
	// スプライト読み込み
	{
		button_ui_base      = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\button_UI.png");
		button_ui_chara     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\UI_unit_sheet.png");
		button_ui_circle    = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\UI_unit_circle.png");

		pause_main          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Pause_kakasi.png");
		pause_text_continue = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Continue.png");
		pause_text_select   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Stageselect.png");

		tutorial_gif[0]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\1.png");
		tutorial_gif[1]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\2.png");
		tutorial_gif[2]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\3.png");
		tutorial_gif[3]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\4.png");
		tutorial_gif[4]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\5.png");
		tutorial_gif[5]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\6.png");
		tutorial_gif[6]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\7.png");
		tutorial_gif[7]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\8.png");
		tutorial_gif[8]     = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\9.png");

		pause_text_continue = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Continue.png");
		pause_text_select   = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Pause\\Stageselect.png");

		button[0]           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Abutton.png");
		button[1]           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Bbutton.png");
		button[2]           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Xbutton.png");
		button[3]           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Ybutton.png");

		menu_text           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\MENUでステージセレクト.png");
		reset_text          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Reset.png");
		Frame_tutorial      = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Frame_tutorial.png");
		
		button_beside       = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Button_beside.png");
		button_up           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Button_up.png");
		
		left_row            = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Leftarrow.png");
		right_row           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Rightarrow.png");
	
		left_text           = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Left.png");
		right_text          = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\Right.png");

		OK = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\OK.png");

		mission_text[MISSION::throw_seed]  = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\たねをとばす.png");
		mission_text[MISSION::change_unit] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\ユニットをかえる.png");
		mission_text[MISSION::move_player] = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\かかしをうごかす.png");
		mission_text[MISSION::attack]      = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Tutorial\\きのこにこうげき.png");
	}
	// ゲーム部分
	{
		// カメラ
		Camera& camera = Camera::Instance();
		camera_range = 33.0f;
		camera_angle = { DirectX::XMConvertToRadians(40),DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(0) };
		camera_target = { -7.0f,0.0f,0.0f };
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
		camera.SetEyeYOffset(8.0f);
		camera.SetAngle(camera_angle);


		// ライティング、色調補正
		// 昼
		directional_light_direction = { -0.342f,-1.00f,0.0f,0.0f };
		option_constant.hsv_adjustment = { 1.0f,1.1f,1.7f,1.0f };
		option_constant.rgb_adjustment = { 1.1f,1.0f,1.0f,1.0f };
		option_constant.parameters.y = 0.5f;
		option_constant.attack_color = { 0.0f,1.0f,1.0f,0.11f };
		option_constant.edge_color = { 0.0f,1.0f,1.0f,0.6f };

		option_constant.attack_color2 = { 1.0f,1.0f,0.0f,0.11f };
		option_constant.edge_color2 = { 1.0f,0.2f,0.0f,1.0f };


		// ステージ初期化
		StageManager& stage_manager = StageManager::Instance();
		StageMain* stage_main = new StageMain;
		stage_manager.Register(stage_main);

		// 初期化
		player = new Player;
		fence = new Fence;
		EnemySpawner::Instance().Initialize();
		UnitManager::Instance().Initialize();
		EnemyManager::Instance().Initialize();

		// ポーズ画面
		is_pause = false;
		is_bloom = true;
		pause_text_continue_scale.value = 1.0f;
		pause_text_select_scale.value = 1.0f;

		// チュートリアル
		tutorial_state = 0;
		tutorial_gif_num = 0;
		ok_text.value = -1920;

		// BGM
		Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_NOON, true);
		Lemur::Audio::AudioManager::Instance().PlayBgm(Lemur::Audio::BGM::GAME_NOON_SE, true);

		// ユニットの決定
		Lemur::Scene::SceneManager::Instance().set_unit_cont[0] = 0;
		Lemur::Scene::SceneManager::Instance().set_unit_cont[0] = 1;
		Lemur::Scene::SceneManager::Instance().set_unit_cont[0] = 2;
		Lemur::Scene::SceneManager::Instance().set_unit_cont[0] = 5;

		// アイリスアウトを呼ぶ
		CallTransition(false);
	}
}

void TutorialScene::Finalize()
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
	// エネミー終了
	EnemyManager::Instance().Clear();
	// ユニット終了
	UnitManager::Instance().Clear();
	// 種終了
	SeedManager::Instance().Clear();
}

void TutorialScene::Update(HWND hwnd, float elapsedTime)
{
	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	// カメラ
	camera.Update(elapsedTime);
	// ライトの更新
	LightUpdate();
	// マスクの更新
	TransitionMask(elapsedTime);

	ok_text.EasingValue(elapsedTime);
	//if (GetAsyncKeyState('K') & 1)
	//{
	//	ok_text.CallValueEasing(0, ok_text.value, ok_text.OutSine, 0.5f);
	//}
	//if (ok_text.value >= 0 && ok_text.value < 1920 && !ok_text.is_easing)
	//{
	//	ok_text.CallValueEasing(1920, ok_text.value, ok_text.InSine, 0.5f);
	//}
	//if (ok_text.value >= 1920 && !ok_text.is_easing)
	//{
	//	ok_text.value = -1920;
	//}

	// ポーズ更新
	if (gamePad.GetButtonUp() & gamePad.BTN_BACK)
	{
		is_pause = !is_pause;
		pause_text_continue_scale.CallValueContinue(1.0f, 1.1f, pause_text_continue_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
	}
	if (is_pause)
	{
		PauseUpdate(elapsedTime);
		return;
	}
	else is_bloom = true;

	// 遷移演出の最中はこれより下に行かない
	if (start_transition)return;

	// ゲーム
	{
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

		// 入力
		HandleInput();

		// エンドレスで敵を出す
		if (EnemyManager::Instance().GetEnemyCount() <= 0&& tutorial_state!= MISSION::attack)
		{
			Enemy* new_enemy = new NuisanceEnemy(false);
			new_enemy->SetHealth(1);
			EnemyManager::Instance().Register(new_enemy);
		}

		// ０番GIF終わったら１番に
		if (tutorial_gif_num == 0 && tutorial_gif[0]->is_anime_end)
		{
			tutorial_gif[0]->is_anime_end = false;
			tutorial_gif_num = 1;
		}

		// 状態の更新
		UpdateState();
	}

	// Imgui
	DebugImgui();
}

void TutorialScene::Render(float elapsedTime)
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
	// TODO もね　敵の大きさ
	const float enemy_scale = 0.016f;

	// 影生成
	{
		ID3D11PixelShader* null_pixel_shader{ NULL };
		SetUpShadowMap();
		shadow_map->Clear(immediate_context);
		shadow_map->Activate(immediate_context);

		// プレイヤー描画
		player->Render(scale, &null_pixel_shader);
		//// ステージ描画
		StageManager::Instance().Render(1.0f, &null_pixel_shader, &null_pixel_shader);
		// ユニット描画
		UnitManager::Instance().Render(scale, null_pixel_shader);
		for (int i = 0; i < UnitManager::Instance().GetUnitCount(); i++)
		{
			UnitManager::Instance().GetUnit(i)->collision_model->Render(scale, null_pixel_shader);
		}
		// エネミー描画
		EnemyManager::Instance().Render(enemy_scale, null_pixel_shader);
		// 種描画
		SeedManager::Instance().Render(0.1f, null_pixel_shader);

		shadow_map->Deactivate(immediate_context);
	}

	// 定数バッファの更新
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
	}
	// ポストエフェクトの開始
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context, 0.2f, 0.3f, 0.5f, 1.0f);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
	}

	//3D描画
	{
		if (enable_deferred)
		{
			// プレイヤー描画
			player->Render(scale, fbx_gbuffer_ps.GetAddressOf());
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

	// エフェクト
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		//TODO もね　当たり判定
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);

		// エフェクト再生
		EffectManager::Instance().Render(view, projection);
	}

	// ポストエフェクトの実行
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}

	if (is_bloom)
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

		GamePad& gamePad = Input::Instance().GetGamePad();
		// 2D描画
		button_ui_base->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 0);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A], SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B], SCREEN_HEIGHT * 1, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X], SCREEN_HEIGHT * 2, SCREEN_WIDTH, SCREEN_HEIGHT);
		button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y], SCREEN_HEIGHT * 3, SCREEN_WIDTH, SCREEN_HEIGHT);

		if (Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A] == player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * 0, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		else if (Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B] == player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * 1, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		else if (Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X] == player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * 2, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);
		else if (Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y] == player->GetCategory()) button_ui_circle->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH * 3, SCREEN_HEIGHT * 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		if(tutorial_gif_num!=0)tutorial_gif[tutorial_gif_num]->Animation(immediate_context, elapsedTime, 0.1f, { 40,70 }, { 600 ,450 }, { 1,1,1,1 }, 0.0f, { 600 ,450 }, tutorial_glf_num_x[tutorial_gif_num]);
		else tutorial_gif[0]->Animation(immediate_context, elapsedTime, 0.05f, { 40,70 }, { 600 ,450 }, { 1,1,1,1 }, 0.0f, { 600 ,450 }, tutorial_glf_num_x[0],false);

		Frame_tutorial->Render(immediate_context, 0, 0, 700, SCREEN_HEIGHT);

		int tutorial_state_index = 0;

		switch (tutorial_state)
		{
		case MISSION::throw_seed:
		case MISSION::throw_seed_end:
			tutorial_state_index = MISSION::throw_seed;
			break;
		case MISSION::move_player:
		case MISSION::move_player_end:
			tutorial_state_index = MISSION::move_player;
			break;
		case MISSION::change_unit:
		case MISSION::change_unit_end:
			tutorial_state_index = MISSION::change_unit;
			break;
		case MISSION::attack:
		case MISSION::attack_end:
			tutorial_state_index = MISSION::attack;
			break;
		}

		mission_text[tutorial_state_index]->Render(immediate_context, 40, 600, 600, 200);

		if(tutorial_gif_num!=8)right_row->Render(immediate_context, 600, 240, 100, 100);
		if(tutorial_gif_num!=0)left_row->Render(immediate_context, -20, 240, 100, 100);

		reset_text->Render(immediate_context, 1600, 960, 300, 100);


		switch (tutorial_state)
		{
		case MISSION::move_player:
		case MISSION::move_player_end:
			button_beside->Animation(immediate_context, elapsedTime, 0.2f, { 330,750 }, { 300 ,300 }, { 1,1,1,1 }, 0.0f, { 300 ,300 }, 3);
			left_text->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
		case MISSION::attack:
		case MISSION::attack_end:
			button_up->Animation(immediate_context, elapsedTime, 0.2f, { 330,750 }, { 300 ,300 }, { 1,1,1,1 }, 180.0f, { 300 ,300 }, 6);
			right_text->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
		case MISSION::throw_seed:
		case MISSION::throw_seed_end:
			button_up->Animation(immediate_context, elapsedTime, 0.2f, { 330,750 }, { 300 ,300 }, { 1,1,1,1 }, 180.0f, { 300 ,300 }, 6);
			right_text->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
			break;
		case MISSION::change_unit:
		case MISSION::change_unit_end:
			for (int i = 0; i < 4; i++)
			{
				button[i]->Render(immediate_context, 55+(140*i), 830, 150, 150);
			}
			break;
		}

		if (tutorial_gif_num >= 6) 
		{
			tutorial_gif[tutorial_gif_num]->Animation(immediate_context, elapsedTime, 0.1f, { 535,150 }, { 600 * 1.8f ,450 * 1.8f }, { 1,1,1,1 }, 0.0f, { 600 ,450 }, tutorial_glf_num_x[tutorial_gif_num]);
		}

		OK->Render(immediate_context, ok_text.value, 0, 1920, 1080);
	}
	else
	{
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);

		pause_main->Render(immediate_context, SCREEN_WIDTH - 820, 0, 820, SCREEN_HEIGHT);
		pause_text_continue->RenderCenter(immediate_context, 1520, 550, 500 * pause_text_continue_scale.value, 120 * pause_text_continue_scale.value);
		pause_text_select->RenderCenter(immediate_context, 1520, 700, 500 * pause_text_select_scale.value, 120 * pause_text_select_scale.value);
	}
	// マスクの描画
	RenderTransitionMask(elapsedTime);
}

void TutorialScene::DebugImgui()
{
#if 0
	ImGui::Begin("tutorial_state");
	switch (tutorial_state)
	{
	case MISSION::throw_seed:
		ImGui::Text("throw_seed");
		break;
	case MISSION::change_unit:
		ImGui::Text("change_unit");
		break;
	case MISSION::move_player:
		ImGui::Text("move_player");
		break;
	case MISSION::clear:
		ImGui::Text("clear");
		break;
	case MISSION::attack:
		ImGui::Text("attack");
		break;
	}
	ImGui::End();
	BaseScene::DebugImgui();
	Camera::Instance().DrawDebug();
#endif
}

void TutorialScene::InitializeLight()
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

void TutorialScene::PauseUpdate(float elapsedTime)
{
	GamePad& gamePad = Input::Instance().GetGamePad();

	is_bloom = false;

	switch (pause_num)
	{
	case 0:
		pause_text_continue_scale.ContinueEasing(elapsedTime);
		pause_text_select_scale.EasingValue(elapsedTime);
		if (gamePad.GetButtonDown() & gamePad.BTN_DOWN)
		{
			pause_text_select_scale.CallValueContinue(1.0f, 1.1f, pause_text_select_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
			pause_text_continue_scale.CallValueEasing(1.0f, pause_text_continue_scale.value, EasingFunction::EasingType::InSine);
			pause_num++;
		}
		else if (gamePad.GetButtonDown() & gamePad.BTN_B)
		{
			is_bloom = true;
			pause_num = 0;
			is_pause = false;
		}
		break;
	case 1:
		pause_text_select_scale.ContinueEasing(elapsedTime);
		pause_text_continue_scale.EasingValue(elapsedTime);
		if (gamePad.GetButtonDown() & gamePad.BTN_UP)
		{
			pause_text_continue_scale.CallValueContinue(1.0f, 1.1f, pause_text_continue_scale.value, EasingFunction::EasingType::OutSine, EasingFunction::EasingType::InSine);
			pause_text_select_scale.CallValueEasing(1.0f, pause_text_select_scale.value, EasingFunction::EasingType::InSine);
			pause_num--;
		}
		else if (gamePad.GetButtonDown() & gamePad.BTN_B)
		{
			is_bloom = true;
			pause_num = 0;
			Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
		}
		break;
	}
}

void TutorialScene::HandleInput()
{
	GamePad& gamePad = Input::Instance().GetGamePad();
	if (gamePad.GetButtonUp() & gamePad.BTN_RIGHT || GetAsyncKeyState(VK_RIGHT) & 1)
	{
		if (tutorial_gif_num < 8)tutorial_gif_num++;
	}
	if (gamePad.GetButtonUp() & gamePad.BTN_LEFT || GetAsyncKeyState(VK_LEFT) & 1)
	{
		if (tutorial_gif_num > 0)tutorial_gif_num--;
	}

	if (gamePad.GetButtonDown() & gamePad.BTN_START||GetAsyncKeyState(VK_BACK) & 1)
	{
		Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new SelectScene));
	}

	if (gamePad.GetButtonUp() & gamePad.BTN_LEFT_SHOULDER || gamePad.GetButtonUp() & gamePad.BTN_RIGHT_SHOULDER|| GetAsyncKeyState(VK_BACK) & 1)
	{
		UnitManager::Instance().Clear();
		SeedManager::Instance().Clear();
	}
}

void TutorialScene::UpdateState()
{
	switch (tutorial_state)
	{
	case MISSION::throw_seed:// 種を飛ばす
		if (SeedManager::Instance().GetSeedCount() > 0)
		{
			ok_text.CallValueEasing(0, ok_text.value, ok_text.OutSine, 0.5f);
			tutorial_state = MISSION::throw_seed_end;
		}
		break;
	case MISSION::throw_seed_end:
		if (ok_text.value >= 0 && ok_text.value < 1920 && !ok_text.is_easing)
		{
			ok_text.CallValueEasing(1920, ok_text.value, ok_text.InSine, 0.5f);
		}
		if (ok_text.value >= 1920 && !ok_text.is_easing)
		{
			ok_text.value = -1920;
			tutorial_gif_num = 2;
			player_current_position = player->GetPosition();
			tutorial_state = MISSION::move_player;
		}
		break;
	case MISSION::move_player:// かかしを動かす
		if (!Equal(player_current_position.x, player->GetPosition().x) || !Equal(player_current_position.z, player->GetPosition().z))
		{
			ok_text.CallValueEasing(0, ok_text.value, ok_text.OutSine, 0.5f);
			tutorial_state = MISSION::move_player_end;
		}
		break;
	case  MISSION::move_player_end:
		if (ok_text.value >= 0 && ok_text.value < 1920 && !ok_text.is_easing)
		{
			ok_text.CallValueEasing(1920, ok_text.value, ok_text.InSine, 0.5f);
		}
		if (ok_text.value >= 1920 && !ok_text.is_easing)
		{
			ok_text.value = -1920;
			tutorial_gif_num = 3;
			unit_category = player->GetCategory();
			tutorial_state = MISSION::change_unit;
		}
		break;
	case MISSION::change_unit:// ユニットを変える
		if (player->GetCategory() != unit_category)
		{
			ok_text.CallValueEasing(0, ok_text.value, ok_text.OutSine, 0.5f);
			tutorial_state = MISSION::change_unit_end;
		}
		break;
	case  MISSION::change_unit_end:
		if (ok_text.value >= 0 && ok_text.value < 1920 && !ok_text.is_easing)
		{
			ok_text.CallValueEasing(1920, ok_text.value, ok_text.InSine, 0.5f);
		}
		if (ok_text.value >= 1920 && !ok_text.is_easing)
		{
			ok_text.value = -1920;
			tutorial_state = MISSION::attack;
		}
		break;
	case MISSION::attack:// 攻撃する
		if (EnemyManager::Instance().GetEnemyCount() <= 0)
		{
			ok_text.CallValueEasing(0, ok_text.value, ok_text.OutSine, 0.5f);
			tutorial_state = MISSION::attack_end;
		}
		break;
	case MISSION::attack_end:
		if (ok_text.value >= 0 && ok_text.value < 1920 && !ok_text.is_easing)
		{
			ok_text.CallValueEasing(1920, ok_text.value, ok_text.InSine, 0.5f);
		}
		if (ok_text.value >= 1920 && !ok_text.is_easing)
		{
			ok_text.value = -1920;
			tutorial_state = MISSION::clear;
		}
		break;

	case MISSION::clear:
		
		break;
	}
}
