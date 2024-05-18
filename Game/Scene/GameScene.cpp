#include "GameScene.h"
#include "./Lemur/Component/DemoEnemyManager.h"
#include "./Lemur/Graphics/Camera.h"
#include "./Lemur/Resource/ResourceManager.h"
#include "./Lemur/Scene/SceneManager.h"
#include "Game/Scene/OverScene.h"
#include "Game/Scene/LoadingScene.h"
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

		// マスクの初期化
		InitializeMask();

		// スカイマップテクスチャのロード
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\winter_evening_4k.hdr", skymap.GetAddressOf(), graphics.GetTexture2D());

		// SHADOW
		shadow_map = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());//TODO

		//TODO 実験用
		create_ps_from_cso(graphics.GetDevice(), "./Shader/sprite_ui.cso", sprite_ui.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_1.cso", stage_ps_1.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps_2.cso", stage_ps_2.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/unit_ps.cso", unit_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/scarecrow.cso", scarecrow_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/enemy_ps.cso", enemy_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/collision_ps.cso", collision.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/fbx_gbuffer_ps.cso", fbx_gbuffer_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_gbuffer_ps.cso", gltf_gbuffer_ps.GetAddressOf());
	}
	// スプライト読み込み
	{
		timer_hands = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\timer_hands.png");
		timer_ui_base =ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\timer_base.png");
		button_ui_base =ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\button_UI.png");
		button_ui_chara =ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\UI\\UI_unit_sheet.png");
	}
	// ゲーム部分
	{
		// カメラ
		Camera& camera = Camera::Instance();
		camera_range = 33.0f;
		camera_angle = { DirectX::XMConvertToRadians(40),DirectX::XMConvertToRadians(0),DirectX::XMConvertToRadians(0) };
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
		camera.SetEyeYOffset(8.0f);
		camera.SetAngle(camera_angle);

		StageManager& stage_manager = StageManager::Instance();
		stage_manager.SetStageLevel(7);
		//TODO もね 制限時間 ステージ選択
		switch (StageManager::Instance().GetStageLevel())
		{
		case 0:// レベル１
			time_limit = 80.0f;// ここで制限時間を調整
			break;
		case 1:// レベル２
			time_limit = 90.0f;// ここで制限時間を調整
			break;
		case 2:// レベル３
			time_limit = 90.0f;// ここで制限時間を調整
			break;
		case 3:// レベル４
			time_limit = 85.0f;// ここで制限時間を調整
			break;
		case 4:// レベル５
			time_limit = 100.0f;// ここで制限時間を調整
			break;
		case 5:// レベル6
			time_limit = 100.0f;// ここで制限時間を調整
			break;
		case 6:// レベル7
			time_limit = 200.0f;// ここで制限時間を調整
			break;
		case 7:// レベル8
			time_limit = 200.0f;// ここで制限時間を調整
			break;
		case 8:// レベル9
			time_limit = 200.0f;// ここで制限時間を調整
			break;
		}

		// ワールドごとのライティング、色調補正
		// 朝
		if (StageManager::Instance().GetStageLevel() == 1 ||
			StageManager::Instance().GetStageLevel() == 2 ||
			StageManager::Instance().GetStageLevel() == 3)
		{
			point_light[1].range = 100;
			directional_light_direction = { 0.737f,-1.00f,-0.795f,0.0f };
			option_constant.hsv_adjustment = { 1.0f,1.1f,1.7f,1.0f };
			option_constant.rgb_adjustment = { 1.0f,1.0f,1.15f,1.0f };
			option_constant.parameters.y = 0.5f;
		}
		// 昼
		if (StageManager::Instance().GetStageLevel() == 3 ||
			StageManager::Instance().GetStageLevel() == 4 ||
			StageManager::Instance().GetStageLevel() == 5)
		{
			directional_light_direction = { -0.342f,-1.00f,0.0f,0.0f };
			option_constant.hsv_adjustment = { 1.0f,1.1f,1.7f,1.0f };
			option_constant.rgb_adjustment = { 1.1f,1.0f,1.0f,1.0f };
			option_constant.parameters.y = 0.5f;
		}
		// 夜
		if (StageManager::Instance().GetStageLevel() == 6 ||
			StageManager::Instance().GetStageLevel() == 7 ||
			StageManager::Instance().GetStageLevel() == 8)
		{
			point_light[0].position = { 0.0f,3.0f,0.0f,1.0f };
			point_light[0].range = 40;
			point_light[1].range = 0;
			directional_light_direction = { 0.111f,-1.00f,-0.676f,0.0f };
			option_constant.hsv_adjustment = { 1.0f,1.1f,1.0f,1.0f };
			option_constant.rgb_adjustment = { 1.0f,1.0f,1.2f,1.0f };
			option_constant.parameters.y = 0.2f;
		}

		// タイマーの初期化
		timer = 0.0f;
		timer_angle = 360 * (timer / time_limit);
		
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
		// エネミーマネージャーの初期化
		EnemyManager::Instance().Initialize();

		// アイリスアウトを呼ぶ
		CallTransition(false);
	}

	// デバッグ
	{
		Try_T.B.y = 2.0;
		Try_T.C.x = 2.0;
		Try_T.C.y = 1.0;
		angle = 50.0f;
		r0 = 0.1f;

		//test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\Jummo\\Jummo.fbx");
		//test_model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\grid.fbx");

		// パーティクルシステム準備
		{
			D3D11_TEXTURE2D_DESC texture2d_desc;
			Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
			// パーティクル用画像ロード
			LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\Particle\\particle256x256.png",
				shader_resource_view.GetAddressOf(), &texture2d_desc);
			// パーティクルシステム生成
			particle_system = std::make_unique<ParticleSystem>(graphics.GetDevice(), shader_resource_view, 4, 4, 10000);

		}
		hitEffect = new Effect(".\\resources\\Effect\\UNIT6_ATK\\UNIT6_ATK_parts.efk");
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
	// 敵スポーン制御装置の初期化
	EnemySpawner::Instance().Finalize();
}

void GameScene::Update(HWND hwnd, float elapsedTime)
{
	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	// カメラ
	{
		camera.Update(elapsedTime);
	}

	// ライトの更新
	LightUpdate();

	// マスクの更新
	TransitionMask(elapsedTime);

	// 遷移演出の最中はこれより下に行かない
	if (start_transition)return;

	// ゲーム
	{
		if (timer >= time_limit)
		{
			EnemyManager::Instance().SetTimeUp(true);
			time_up = true;
		}
		if (time_up)
		{
			EnemyManager::Instance().PowerUpEnemy();
			// タイムアップかつ敵が全て死んだら
			if (EnemyManager::Instance().GetEnemyCount() <= 0)Lemur::Scene::SceneManager::Instance().ChangeScene(new LoadingScene(new OverScene));
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

	// デバッグ
	{
		if (::GetAsyncKeyState('A') & 0x8000)
		{
			hitEffect->Play({ 0,0,0 },0.3f);
		}
		//// パーティクルシステム更新
		//if (particle_system)
		//{
		//	if (::GetAsyncKeyState('A') & 0x8000)
		//	{
		//		DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3((
		//			rand() % 30 - 15) * 0.1f,
		//			rand() % 30 * 0.1f + 20,
		//			(rand() % 30 - 15) * 0.1f + 3);
		//		int max = 100;
		//		for (int i = 0; i < max; i++)
		//		{
		//			// 発生位置
		//			DirectX::XMFLOAT3 p = { 0,0,0 };
		//			p.x = pos.x + (rand() % 10001 - 5000) * 0.01f;
		//			p.y = pos.y;
		//			p.z = pos.z + (rand() % 10001 - 5000) * 0.01f;
		//			// 発生方向
		//			DirectX::XMFLOAT3 v = { 0,0,0 };
		//			v.y = -(rand() % 10001) * 0.0005f - 0.002f;
		//			// 力
		//			DirectX::XMFLOAT3 f = { 0,0,0 };
		//			f.x = (rand() % 10001) * 0.0001f + 0.1f;
		//			f.z = (rand() % 10001 - 5000) * 0.00001f;
		//			// 大きさ
		//			DirectX::XMFLOAT2 s = { .2f,.2f };
		//			particle_system->Set(12, 5, p, v, f, s);
		//		}
		//	}
		//	particle_system->Update(elapsedTime);
		//}
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
	SetUpConstantBuffer();

	immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());

	// スケール
	const float scale = 0.015f;
	// TODO もね　えねみーの大きさ
	const float enemy_scale = 0.016f;

	SetUpShadowMap();

	shadow_map->Clear(immediate_context);
	shadow_map->Activate(immediate_context);

	ID3D11PixelShader* null_pixel_shader{ NULL };
	// プレイヤー描画
	player->Render(scale, &null_pixel_shader);

	//// ステージ描画
	StageManager::Instance().Render(1.0f, &null_pixel_shader, &null_pixel_shader);
	// ユニット描画
	UnitManager::Instance().Render(scale, null_pixel_shader);
	for (int i = 0; i < UnitManager::Instance().GetUnitCount(); i++)
	{
		//UnitManager::Instance().GetUnit(i)->collision_model->Render(scale, unit_ps.Get());
	}
	// エネミー描画
	EnemyManager::Instance().Render(enemy_scale, null_pixel_shader);
	// 種描画
	SeedManager::Instance().Render(0.1f, null_pixel_shader);

	shadow_map->Deactivate(immediate_context);

	SetUpConstantBuffer();
	// エッジ検出
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Activate(immediate_context);
		ID3D11PixelShader* null_pixel_shader{ NULL };
		player->Render(0.025f, &null_pixel_shader);
		UnitManager::Instance().CollisionRender(scale, null_pixel_shader);
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
		immediate_context->PSSetShaderResources(8, 1, shadow_map->shader_resource_view.GetAddressOf());
		//　深度値
		immediate_context->PSSetShaderResources(11/*Edge*/, 1, framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->shader_resource_views[1].GetAddressOf());
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
			UnitManager::Instance().CollisionRender(scale, collision.Get());
			for (int i = 0; i < UnitManager::Instance().GetUnitCount(); i++)
			{
				//UnitManager::Instance().GetUnit(i)->collision_model->Render(scale, unit_ps.Get());
			}
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


	//TODO debug
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		//TODO もね　当たり判定
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);

		// エフェクト再生
		EffectManager::Instance().Render(view, projection);

		// ブレンドステート設定
		immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステート設定
		immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 0);
		// ラスタライザーステート設定
		immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		if (particle_bomb)	particle_bomb->Render(immediate_context);
		if (particle_system)	particle_system->Render(immediate_context);
	}
	// ポストエフェクトの実行
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}

	GamePad& gamePad = Input::Instance().GetGamePad();
	// 2D描画
	button_ui_base->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,0);
	button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH*Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.A], SCREEN_HEIGHT*0, SCREEN_WIDTH, SCREEN_HEIGHT);
	button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH*Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.B], SCREEN_HEIGHT*1, SCREEN_WIDTH, SCREEN_HEIGHT);
	button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH*Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.X], SCREEN_HEIGHT*2, SCREEN_WIDTH, SCREEN_HEIGHT);
	button_ui_chara->Render(immediate_context, sprite_ui.Get(), 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0, SCREEN_WIDTH*Lemur::Scene::SceneManager::Instance().set_unit_cont[gamePad.Y], SCREEN_HEIGHT*3, SCREEN_WIDTH, SCREEN_HEIGHT);

	timer_ui_base->Render(immediate_context, sprite_ui.Get(), 50, 50, 214, 273, 0.0f);
	timer_hands->RenderCenter(immediate_context, sprite_ui.Get(), 50 + 107, 50 + 167, 4, 180, timer_angle);
	
	// マスクの描画
	RenderTransitionMask(elapsedTime);
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
