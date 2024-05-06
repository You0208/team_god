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
		double_speed_z = std::make_unique<ShadowMap>(graphics.GetDevice(), shadowmap_width, shadowmap_height);
		// dissolve
		LoadTextureFromFile(graphics.GetDevice(), L".\\resources_2\\Image\\dissolve_animation.png", noise.GetAddressOf(), graphics.GetTexture2D());//TODO

		//TODO 実験用
		create_ps_from_cso(graphics.GetDevice(), "./Shader/chara_model_ps.cso", Try.GetAddressOf());
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
		test_model = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\spider_v009.fbx");
		test_model_2 = std::make_unique<FbxModelManager>(graphics.GetDevice(), ".\\resources_2\\Model\\grid.fbx");

		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\glTF-Sample-Models-master\\2.0\\FlightHelmet\\glTF\\FlightHelmet.gltf");
		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\glTF-Sample-Models-master\\2.0\\DamagedHelmet\\glTF\\DamagedHelmet.gltf");
		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\spider_v009.glb", false);
		gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit3_RE.glb",true);
		//gltf_test_model = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources\\Model_glb\\Unit\\unit1_RE.glb");
		gltf_test_model_2 = std::make_unique<GltfModelManager>(graphics.GetDevice(), ".\\resources_2\\glTF-Sample-Models-master\\2.0\\TwoSidedPlane\\glTF\\TwoSidedPlane.gltf",false);	

		result = ResourceManager::Instance().load_sprite_resource(graphics.GetDevice(), L".\\resources\\Image\\Formation_scene.png");

		debugEffect = new Effect(".\\resources\\Effect\\UNIT4_BUFF\\UNIT4_BUFF.efk");
	}

	// ポイントライト・スポットライトの初期位置設定
	InitializeLight();

	// デバッグ
	{
		gltf_test_model->PlayAnimation(0, true);
		test_model->PlayAnimation(0, true);
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
		gltf_test_model->UpdateAnimation(elapsedTime);
		test_model->UpdateAnimation(elapsedTime);
		if (gamePad.GetButtonDown() & gamePad.BTN_A)
		{
			gltf_test_model->PlayAnimation(0, true);
			test_model->PlayAnimation(0, true);
			debugEffect->Play(test_model->GetTransform()->GetPosition(),0.1);
		}
		if (gamePad.GetButtonDown() & gamePad.BTN_B)
		{
			gltf_test_model->PlayAnimation(1, true);
			test_model->PlayAnimation(1, true);
		}
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

		//ImGui::SliderFloat("fbx_animation_blend_time", &test_model->animation_blend_time, 0.0f, 1.00f);

		ImGui::SliderFloat("animation_blend_time", &gltf_test_model->animation_blend_time, 0.0f, 10.00f);
		ImGui::SliderFloat("blend_rate", &gltf_test_model->blend_rate, 0.0f, 1.00f);

		// STATIC_BATCHING
		if (ImGui::TreeNode("shadow"))
		{
			ImGui::Image(reinterpret_cast<void*>(double_speed_z->shader_resource_view.Get()), ImVec2(shadowmap_width / 5.0f, shadowmap_height / 5.0f));
			ImGui::SliderFloat("shadow_depth_bias", &scene_constants.shadow_depth_bias, 0.1f, 0.01f);
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
	if (enable_deferred)
	{
		//test_model->Render(0.01f, fbx_gbuffer_ps.Get());
		//test_model_2->Render(0.1f, fbx_gbuffer_ps.Get());
		//test_model->DrawDebug("Test");
		//test_model_2->DrawDebug("Test_2");

		gltf_test_model->Render(1.0f, gltf_gbuffer_ps.Get());
		gltf_test_model_2->Render(1.0f, gltf_gbuffer_ps.Get());
	}
	else
	{

		//test_model->Render(0.01f, Try.Get());
		//test_model_2->Render(0.1f, Try.Get());
		//test_model->DrawDebug("Test");
		//test_model_2->DrawDebug("Test");

		gltf_test_model->Render(1.0f, gltf_ps.Get());
		//gltf_test_model_2->Render(1.0f, gltf_ps.Get());
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

	// ステートの設定
	//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_OFF_ZW_OFF)].Get(), 0);
	//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::CULL_NONE)].Get());
	//result->Render(immediate_context, 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
	
	RenderingDeffered();
	
	
	// ポストエフェクトの実行
	if (enable_post_effect)
	{
		framebuffers[static_cast<size_t>(FRAME_BUFFER::SCENE)]->Deactivate(immediate_context);
		ExePostEffct();
	}
	//test_model->Render(0.01f, fbx_gbuffer_ps.Get());
	//immediate_context->OMSetDepthStencilState(depth_stencil_states[static_cast<size_t>(DEPTH_STATE::ZT_ON_ZW_ON)].Get(), 0);
	//immediate_context->RSSetState(rasterizer_states[static_cast<size_t>(RASTER_STATE::SOLID)].Get());
	//gltf_test_model->Render(1.0f, gltf_ps.Get());
}
