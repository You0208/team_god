#include "DemoScene.h"
#include "./Lemur/Component/DemoEnemyManager.h"
#include "../Graphics/Camera.h"
#include "../Resource/ResourceManager.h"
#include "../Scene/SceneManager.h"
#include "./high_resolution_timer.h"
// Effect
#include "../Effekseer/EffekseerManager.h"

//#define USE_DEFEFFERD
//#define POST_EFFECT
//#define DEFEFFERD_POST_EFFECT

void DemoScene::Initialize()
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

		// スカイマップテクスチャのロード
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\winter_evening_4k.hdr", skymap.GetAddressOf(), graphics.GetTexture2D());

		// SHADOW
		shadow_map = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());//TODO

		//TODO 実験用
		create_ps_from_cso(graphics.GetDevice(), "./Shader/skinned_mesh_ps.cso", Try.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", chara_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/stage_model_ps.cso", stage_ps.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_chara_ps.cso", gltf_ps.GetAddressOf());

		create_ps_from_cso(graphics.GetDevice(), "./Shader/fbx_gbuffer_ps.cso", fbx_gbuffer_ps.GetAddressOf());
		create_ps_from_cso(graphics.GetDevice(), "./Shader/gltf_gbuffer_ps.cso", gltf_gbuffer_ps.GetAddressOf());
	}
	// ゲーム部分
	{
		Camera& camera = Camera::Instance();

		// STATIC_BATCHING
		//static_meshes[0] = std::make_unique<StaticMeshBatch>(device.Get(), ".\\resources_2\\terrain\\terrain.fbx", true);
		//static_meshes[0] = std::make_unique<StaticMeshBatch>(graphics.GetDevice(), ".\\resources_2\\death_valley_-_terrain.fbx", true);
		//skinned_meshes[0] = std::make_unique<SkinnedMesh>(graphics.GetDevice(), ".\\resources_2\\Model\\nico.fbx", true);
		//skinned_meshes[0] = std::make_unique<SkinnedMesh>(graphics.GetDevice(), ".\\resources_2\\ball\\ball.obj", true);
		//skinned_meshes[1] = std::make_unique<SkinnedMesh>(graphics.GetDevice(), ".\\resources_2\\Model\\plane\\plane.obj", true);
		//sprite_batches[0] = std::make_unique<SpriteBatch>(graphics.GetDevice(), L".\\resources_2\\screenshot.jpg", 1);

		//skinned_meshes[1] = std::make_unique<SkinnedMesh>(graphics.GetDevice(), ".\\resources_2\\Model\\latha.fbx", true);
		//skinned_meshes[2] = std::make_unique<SkinnedMesh>(graphics.GetDevice(), ".\\resources_2\\Try\\try_2.fbx", true);
		//skinned_meshes[2] = std::make_unique<SkinnedMesh>(graphics.GetDevice(), ".\\resources_2\\Model\\fantasy_game_inn\\source\\theInn.FBX.fbx", true);

		//gltf_models[0] = std::make_unique<GltfModel>(graphics.GetDevice(), 
		//	".\\resources_2\\glTF-Sample-Models-master\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf");
		//gltf_models[0] = std::make_unique<GltfModel>(graphics.GetDevice(), ".\\resources_2\\french_street_low_poly\\scene.gltf");
		//gltf_models[0] = std::make_unique<GltfModel>(graphics.GetDevice(), ".\\resources_2\\wizards_library\\scene.gltf");
		//gltf_models[1] = std::make_unique<GltfModel>(graphics.GetDevice(), ".\\resources_2\\toon_cat_free\\scene.gltf");

		//gltf_models[0] = std::make_unique<GltfModel>(graphics.GetDevice(),
		//	".\\resources_2\\Chili_24_0303_01\\Chili_24_0303_01.glb");
		//test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\Jummo\\Jummo.fbx");
		//test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\spider_v009.fbx");
		test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\cave\\cave_set.fbx");
		//test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Stage\\tree\\tree.fbx");
		//test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources\\Model\\Enemy\\Boar.fbx");
		//test_model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\grid.fbx");

		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\glTF-Sample-Models-master\\2.0\\FlightHelmet\\glTF\\FlightHelmet.gltf");
		gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\glTF-Sample-Models-master\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf",false);
		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\spider_v009.glb", false);
		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\Chili.glb",true);
		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit1_RE.glb");
		gltf_test_model_2 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\glTF-Sample-Models-master\\2.0\\TwoSidedPlane\\glTF\\TwoSidedPlane.gltf",false);	

		result = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Result\\result_back.png");

		debugEffect = new Effect(".\\resources\\Effect\\UNIT3_ATK\\UNIT3_ATK.efk");
	}
	sprite_batches[0] = std::make_unique<SpriteBatch>(graphics.GetDevice(), L".\\resources_2\\screenshot.jpg", 1);


	// ポイントライト・スポットライトの初期位置設定
	InitializeLight();

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
	// 爆発アニメーション付きパーティクル
	{
		D3D11_TEXTURE2D_DESC texture2d_desc;
		Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> shader_resource_view;
		// パーティクル用画像ロード
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\Particle\\Explosion01_5x5.png",
			shader_resource_view.GetAddressOf(), &texture2d_desc);
		// パーティクルシステム生成
		particle_bomb = std::make_unique<ParticleSystem>(graphics.GetDevice(), shader_resource_view, 5, 5);
	}

	// デバッグ
	{
		directional_light_direction = { 1,-1,-1,0 };
		gltf_test_model->PlayAnimation(0, false);
		test_model->PlayAnimation(0, false);
	}
}

void DemoScene::Finalize()
{
	//エネミー終了
	DemoEnemyManager::Instance().Clear();
}

void DemoScene::Update(HWND hwnd, float elapsedTime)
{
	using namespace DirectX;
	Camera& camera = Camera::Instance();
	GamePad& gamePad = Input::Instance().GetGamePad();
	// エフェクト更新処理
	EffectManager::Instance().Update(elapsedTime);

	// カメラ
	{
		camera.Update(elapsedTime);
		camera.SetTarget(camera_target);
		camera.SetRange(camera_range);
	}

	{
		gltf_test_model->gltf_model->DebugThreshold();

		gltf_test_model->UpdateAnimation(elapsedTime);
		test_model->UpdateAnimation(elapsedTime);
		if (gamePad.GetButtonDown() & gamePad.BTN_A)
		{
			//gltf_test_model->PlayAnimation(0, false);
			//gltf_test_model->PlayAnimation(0, true);
			//test_model->PlayAnimation(0, false);
			debugEffect->Play_R(test_model->GetTransform()->GetPosition(),0.1f,DirectX::XMConvertToRadians(90.0f));
		}
		if (gamePad.GetButtonDown() & gamePad.BTN_B)
		{
			gltf_test_model->PlayAnimation(1, false);
			//gltf_test_model->PlayAnimation(1, true);
			//test_model->PlayAnimation(1, true);
		}
	}

	// パーティクルシステム更新
	if (particle_system)
	{
		// パーティクル生成:単純に出して上に移動させる
		if (::GetAsyncKeyState('Z') & 0x8000)
		{
			particle_system->Set(
				0, 1.0f,
				DirectX::XMFLOAT3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1, (rand() % 30 - 15) *
					0.1f + 3),
				DirectX::XMFLOAT3(
					(rand() % 200 - 100) * 0.001f,
					(rand() % 100) * 0.005f,
					0),
				DirectX::XMFLOAT3(0, 0.5f, 0),
				DirectX::XMFLOAT2(3.0f, 3.0f)
			);
		}
		// パーティクル生成:降雪
		if (::GetAsyncKeyState('X') & 0x8000)
		{
			DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1,
				(rand() % 30 - 15) * 0.1f + 3);
			int max = 100;
			for (int i = 0; i < max; i++)
			{
				// 発生位置
				DirectX::XMFLOAT3 p = { 0,0,0 };
				p.x = pos.x + (rand() % 10001 - 5000) * 0.01f;
				p.y = pos.y;
				p.z = pos.z + (rand() % 10001 - 5000) * 0.01f;
				// 発生方向
				DirectX::XMFLOAT3 v = { 0,0,0 };
				v.y = -(rand() % 10001) * 0.0002f - 0.002f;
				// 力
				DirectX::XMFLOAT3 f = { 0,0,0 };
				f.x = (rand() % 10001) * 0.00001f + 0.1f;
				f.z = (rand() % 10001 - 5000) * 0.00001f;
				// 大きさ
				DirectX::XMFLOAT2 s = { .2f,.2f };
				particle_system->Set(12, 5, p, v, f, s);
			}
		}
		// パーティクル生成:スパーク
		if (::GetAsyncKeyState('C') & 0x8000)
		{
			DirectX::XMFLOAT3 pos = DirectX::XMFLOAT3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1,
				(rand() % 30 - 15) * 0.1f + 3);
			int max = 100;
			for (int i = 0; i < max; i++)
			{
				DirectX::XMFLOAT3 p;
				p.x = pos.x;
				p.y = pos.y;
				p.z = pos.z;
				DirectX::XMFLOAT3 v = { 0,0,0 };
				v.x = (rand() % 10001 - 5000) * 0.0001f;
				v.y = (rand() % 10001) * 0.0002f + 1.2f;
				v.z = (rand() % 10001 - 5000) * 0.0001f;
				DirectX::XMFLOAT3 f = { 0,-1.2f,0 };
				DirectX::XMFLOAT2 s = { 0.05f,0.05f };
				particle_system->Set(2, 3, p, v, f, s);
			}
		}
		particle_system->Update(elapsedTime);
	}
	// パーティクルシステム更新
	if (particle_bomb)
	{
		// 爆発演出
		if (::GetAsyncKeyState('A') & 0x8000)
		{
			particle_bomb->Set(
				0, 1.0f,
				DirectX::XMFLOAT3((rand() % 30 - 15) * 0.1f, rand() % 30 * 0.1f + 1, (rand() % 30 - 15) *
					0.1f + 3),
				DirectX::XMFLOAT3(
					(rand() % 200 - 100) * 0.001f,
					(rand() % 100) * 0.005f,
					0),
				DirectX::XMFLOAT3(0, 0.5f, 0),
				DirectX::XMFLOAT2(3.0f, 3.0f),
				true,
				24.0f
			);
		}
		particle_bomb->Update(elapsedTime);
	}

	// ライトの更新
	LightUpdate();

	//---------------------------------------------------------------------------------------
	// Imgui
	//---------------------------------------------------------------------------------------
	{
		BaseScene::DebugImgui();
		Camera::Instance().DrawDebug();
		ImGui::Begin("ImGUI");

		if (gltf_test_model->IsPlayAnimation())
		{
			ImGui::Text("Play");
		}
		else
		{
			ImGui::Text("End");
		}

		//ImGui::SliderFloat("fbx_animation_blend_time", &test_model->animation_blend_time, 0.0f, 1.00f);

		ImGui::SliderFloat("animation_blend_time", &gltf_test_model->animation_blend_time, 0.0f, 10.00f);
		ImGui::SliderFloat("blend_rate", &gltf_test_model->blend_rate, 0.0f, 1.00f);

		// STATIC_BATCHING
		if (ImGui::TreeNode("shadow"))
		{
			ImGui::Image(reinterpret_cast<void*>(shadow_map->shader_resource_view.Get()), ImVec2(shadowmap_width / 5.0f, shadowmap_height / 5.0f));
			ImGui::SliderFloat("shadow_depth_bias", &scene_constant.shadow_depth_bias, 0.1f, 0.01f);
			ImGui::TreePop();
		}
		ImGui::End();
	}
}

void DemoScene::Render(float elapsedTime)
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

	// エッジ検出
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Activate(immediate_context);
		ID3D11PixelShader* null_pixel_shader{ NULL };
		test_model->Render(0.01f, null_pixel_shader);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::DEPTH)]->Deactivate(immediate_context);
	}

	// ディファードレンダリングの設定
	SetUpDeffered();

	//ステートの設定
	if(enable_deferred)immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::MLT_ALPHA)].Get(), nullptr, 0xFFFFFFFF);
	else immediate_context->OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
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
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Clear(immediate_context);
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Activate(immediate_context);
	}

	sprite_batches[0]->Begin(immediate_context);
	//sprite_batches[0]->Render(immediate_context, 0, 0, 1280, 720);
	sprite_batches[0]->End (immediate_context);

	//3D描画
	if (enable_deferred)
	{
		//test_model->Render(0.01f, fbx_gbuffer_ps.Get());
		//test_model_2->Render(0.1f, fbx_gbuffer_ps.Get());
		//test_model->DrawDebug("Test");
		//test_model_2->DrawDebug("Test_2");

		//gltf_test_model->Render(1.0f, gltf_gbuffer_ps.Get());
		//gltf_test_model_2->Render(1.0f, gltf_gbuffer_ps.Get());
	}
	else
	{
		//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
		//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());
		test_model->Render(0.1f, Try.Get());
		//test_model_2->Render(0.1f, Try.Get());
		//test_model->DrawDebug("Test");
		//test_model_2->DrawDebug("Test");
		//gltf_test_model->GetTransform()->SetPositionY(1.0f);
		//gltf_test_model->Render(1.0f, gltf_ps.Get());
		//gltf_test_model_2->Render(100.0f, gltf_ps.Get());
	}
	// デバッグ
	{
		DirectX::XMFLOAT4X4 view;
		DirectX::XMFLOAT4X4 projection;
		DirectX::XMStoreFloat4x4(&view, camera.GetViewMatrix());
		DirectX::XMStoreFloat4x4(&projection, camera.GetProjectionMatrix());
		graphics.GetDebugRenderer()->Render(immediate_context, view, projection);
		EffectManager::Instance().Render(view, projection);
	}

	//// ステートの設定
	//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	//result->Animation(immediate_context, { 0, 0 }, { SCREEN_WIDTH, SCREEN_HEIGHT }, { 1,1,1,1 }, 0.0f, { SCREEN_WIDTH, SCREEN_HEIGHT }, 3, true);
	////game_over_back->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, 1, 1, 1, 1, 0.0f, SCREEN_WIDTH * 7, SCREEN_HEIGHT*3, SCREEN_WIDTH, SCREEN_HEIGHT);
	//
	RenderingDeffered();
	
	// ポストエフェクトの実行
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}

	// パーティクル描画
	{
		// ブレンドステート設定
		immediate_context -> OMSetBlendState(blend_states[static_cast<size_t>(BLEND_STATE::ALPHA)].Get(), nullptr, 0xFFFFFFFF);
		// 深度ステンシルステート設定
		immediate_context -> OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_OFF)].Get(), 0);
		// ラスタライザーステート設定
		immediate_context -> RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
		if (particle_system)	particle_system->Render(immediate_context);
		if (particle_bomb)	particle_bomb->Render(immediate_context);
	}
	//test_model->Render(0.01f, fbx_gbuffer_ps.Get());
	//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());
	//gltf_test_model->Render(1.0f, gltf_ps.Get());
}

void DemoScene::InitializeLight()
{
}
